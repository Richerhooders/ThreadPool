#include "threadpool.h"


const int TASK_MAX_THRESHHOLD = 1024;
const int THREAD_MAX_THRESHHOLD = 100;
const int THREAD_MAX_IDLE_TIME = 60; // 单位：秒

//线程池构造
ThreadPool::ThreadPool():initThreadSize_(4),
                         taskSize_(0),
                         taskQueMaxThreshHold_(TASK_MAX_THRESHHOLD),
                         threadSizeThresdHold_(THREAD_MAX_THRESHHOLD),
                         curThreadSize_(0),
                         poolMode_(PoolMode::MODE_FIXED),
                         isPoolRunning_(false),
                         idleThreadSize_(0)
                         {}

//线程池析构
ThreadPool::~ThreadPool(){
    isPoolRunning_ = false;
    // notEmpty_.notify_all();

    //等待线程池里面所有的线程返回 有两种状态：阻塞 正在执行任务中,第三情况：可能发生死锁，线程刚进入循环处于拿锁状态
    std::unique_lock<std::mutex> lock(taskQueMtx_);
    notEmpty_.notify_all(); //避免死锁，比如线程池线程先拿到锁进入的等待的时候，还有机会去唤醒。
    exitCond_.wait(lock,[&]() -> bool{return threads_.size() == 0;});//主线程(用户线程阻塞在这里等待线程池中的线程回收)
    
}

//设置线程池的工作模式
void ThreadPool::setMode(PoolMode mode){
    if(checkRunningState()) {
        return; 
    }
    poolMode_ = mode;
}

//设置task任务队列上限阈值
void ThreadPool::setTaskQueMaxThreshHold(int threshhold){
    if(checkRunningState()) {
        return; 
    }
    taskQueMaxThreshHold_ = threshhold;
}

//设置线程池cached模式下线程阈值
void ThreadPool::setThreadSizeThreshHold(int threshhold) {
    if(checkRunningState()) {
        return;
    }
    if(poolMode_ == PoolMode::MODE_CACHED) {
        threadSizeThresdHold_ = threshhold;
    }
}

//给线程池提交任务  用户调用该接口，传入任务对象，生产任务
Result ThreadPool::submitTask(std::shared_ptr<Task> sp){
    //获取锁
    std::unique_lock<std::mutex> lock(taskQueMtx_);
    //线程通信 等待任务队列有空余
    // while(taskQue_.size() == taskQueMaxThreshHold_) {
    //     notFull_.wait(lock);
    // }
    //taskQue_队列满则阻塞，用户提交任务最长不能阻塞1s，否则判断提交任务失败，返回
    // notFull_.wait(lock,[&]() -> bool {return taskQue_.size() < taskQueMaxThreshHold_;});
    if(!notFull_.wait_for(lock,std::chrono::seconds(1),
                    [&]() -> bool {return taskQue_.size() < (size_t) taskQueMaxThreshHold_;})) {
        //表示notFull_等待1s中，条件依然没有满足。
        std::cerr << "task queue is full, submit task fail." << std::endl;
        return Result(sp,false); 
        // return task -> getResult();//不能用,因为在线程拿到任务后，任务从队列中弹出，线程执行完任务，任务随后被析构，所以Result不能依赖于Task
    }
    //如果有空余，把任务放到任务队列中
    taskQue_.emplace(sp);
    taskSize_++;
    //因为新放了任务，任务队列不为空，在notEmpty上进行通知，赶快分配线程执行任务
    notEmpty_.notify_all();

    //cached模式，任务处理比较紧急 场景：小而快的任务需要根据任务数量和空闲线程数量，判断是否需要新的线程出来
    if(poolMode_ == PoolMode::MODE_CACHED 
            && taskSize_ > idleThreadSize_
            && curThreadSize_ < threadSizeThresdHold_) {
        std::cout << ">>> create new thread ..." << std::endl;
        //创建新线程
        std::unique_ptr<Thread> ptr = std::make_unique<Thread>(std::bind(&ThreadPool::threadFunc,this,std::placeholders::_1));
        int threadId = ptr -> getId();
        threads_.emplace(threadId,std::move(ptr));
        //启动线程
        threads_[threadId] -> start();
        //修改线程个数相关数量
        curThreadSize_++;
        idleThreadSize_++;
    }

    return Result(sp);
    // return task -> getResult();

}

//开启线程池
void ThreadPool::start(int initThreadSize){
    //设置线程池的运行状态
    isPoolRunning_ = true;

    //记录初始线程个数
    initThreadSize_ = initThreadSize;
    curThreadSize_ = initThreadSize;

    //创建线程对象
    for(int i = 0;i < initThreadSize_;i++) {
        std::unique_ptr<Thread> ptr = std::make_unique<Thread>(std::bind(&ThreadPool::threadFunc,this,std::placeholders::_1));
        //创建thread线程对象的时候，把线程函数给到thread线程对象
        //用于创建一个新的可调用对象，将ThreadPool 类的成员函数 threadFunc 和当前 ThreadPool 对象的实例（通过 this 指针）绑定在一起。
        //该对象用ThreadFunc接收，见Thread的构造函数。
        // threads_.emplace_back(std::move(ptr));//注意！
        int threadId = ptr -> getId();
        threads_.emplace(std::make_pair(threadId,std::move(ptr)));
    }

    //启动所有线程
    for(int i = 0;i < initThreadSize_;i++) {
        threads_[i] -> start();//需要去执行一个线程函数
        idleThreadSize_++;//记录初始空闲现场的数量
    }
}

//定义线程函数 线程池的所有线程从任务队列里面消费任务
void ThreadPool::threadFunc(int threadid){ //线程函数返回，相应的线程也就结束了
    auto lastTime = std::chrono::high_resolution_clock().now();
    //所有任务必须执行完成，线程池才可以回收所有资源
    for(;;){
        std::shared_ptr<Task> task;
        {
            //先获取锁
            std::unique_lock<std::mutex> lock(taskQueMtx_);

            std::cout << "tid" << std::this_thread::get_id() << "尝试获取任务" << std::endl;

            //有任务不会回收资源，必须任务执行完成任务队列为空
            while(taskQue_.size() == 0) {
                //线程池要结束，回收线程资源
                if(!isPoolRunning_) { //回收资源
                    threads_.erase(threadid);
                    std::cout << "threadid:" << std::this_thread::get_id() << " exit!" << std::endl;
                    exitCond_.notify_all(); //通知主线程（用户线程）退出
                    return; //线程函数结束，线程结束。
                }
                //在cache模式下，可能已经创建了很多的线程，但是空闲时间超过60s的话，应该把多余的线程结束回收掉
                //超过initThreadSize_数量的线程要进行回收
                //当前时间-上一次线程执行的时间 超过 60s
                if(poolMode_ == PoolMode::MODE_CACHED) {
                    //每一秒返回一次 怎么区分超时返回和有任务待执行返回
                    //条件变量超时返回了
                    if(std::cv_status::timeout == notEmpty_.wait_for(lock,std::chrono::seconds(1))) {
                        auto now = std::chrono::high_resolution_clock().now();
                        auto dur = std::chrono::duration_cast<std::chrono::seconds>(now - lastTime);
                        if(dur.count() >= THREAD_MAX_IDLE_TIME && curThreadSize_ > initThreadSize_) {
                            //开始回收当前线程
                            //记录线程数量的相关变量的值修改
                            //把线程对象从线程列表容器中删除，怎么删除当前的线程对应的线程对象，怎么根据ThreadFunc找到Thread对象？方法：增加threadId
                            threads_.erase(threadid);
                            curThreadSize_--;
                            idleThreadSize_--;
                            std::cout << "threadid:" << std::this_thread::get_id() << "exit!" << std::endl;
                            return;
                            
                        }
                    }
                }
                else{
                    //等待notEmpty条件
                    notEmpty_.wait(lock);
                }
            }

            idleThreadSize_--;

            std::cout << "tid" << std::this_thread::get_id() << "获取任务成功" << std::endl;
            //从任务队列中取一个任务出来
            task = taskQue_.front();
            taskQue_.pop();
            taskSize_--;
            //如果依然有剩余任务，继续通知其他线程执行任务
            if(taskQue_.size() > 0) {
                notEmpty_.notify_all();
            }
            //取出一个任务，进行通知,通知可以继续提交生产任务。
            notFull_.notify_all();
        }//就应该把锁释放掉,不能让线程拿着锁去执行任务！

        //当前线程负责执行这个任务 
        if(task != nullptr) {
            // task->run();//执行任务
            //把任务的返回值通过setVal给到Result
            task->exec();
        }
        idleThreadSize_++;
        lastTime = std::chrono::high_resolution_clock().now(); //更新线程执行完任务的时间
    }
}

bool ThreadPool::checkRunningState() const {
    return isPoolRunning_;
}

///////////线程方法实现
int Thread::generateId_ = 0;

//线程构造
Thread::Thread(ThreadFunc func):func_(func),threadId_(generateId_++){}

//线程析构
Thread::~Thread(){}
//启动线程
void Thread::start() {
    //创建一个线程来执行一个线程函数
    std::thread t(func_,threadId_);// C++11来说，有线程对象t和线程函数func_
    t.detach();//设置分离对象
}

int Thread::getId() const {
    return threadId_;
}

///////// Task方法实现
Task::Task() : result_(nullptr) {}

void Task::exec() {
    if(result_ != nullptr) {
        result_-> setVal(run());// 这里发生多态调用
    }
}
void Task::setResult(Result* res){//指向Result对象的指针
    result_ = res;
}

///////// Result方法实现
Result::Result(std::shared_ptr<Task>task, bool isValid):task_(task),isValid_(isValid) {
    task_ -> setResult(this);
}

Any Result::get() {
    if(!isValid_) {
        return "";
    }
    sem_.wait(); //task任务如果没有执行完，这会阻塞用户的线程
    return std::move(any_);
}


void Result::setVal(Any any){
    //存储task的返回值
    this -> any_ = std::move(any);
    sem_.post();//已经获取了任务的返回值，增加信号量资源
}
