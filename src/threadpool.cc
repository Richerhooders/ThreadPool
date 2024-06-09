#include "threadpool.h"


const int TASK_MAX_THREASHHOLD = 1024;

//线程池构造
ThreadPool::ThreadPool():initThreadSize_(4),
                         taskSize_(0),
                         taskQueMaxThreshHold_(TASK_MAX_THREASHHOLD),
                         poolMode_(PoolMode::MODE_FIXED)
                         {}

//线程池析构
ThreadPool::~ThreadPool(){
}

//设置线程池的工作模式
void ThreadPool::setMode(PoolMode mode){
    poolMode_ = mode;
}

//设置task任务队列上限阈值
void ThreadPool::setTaskQueMaxThreshHold(int threshhold){
    taskQueMaxThreshHold_ = threshhold;
}

//给线程池提交任务  用户调用该接口，传入任务对象，生产任务
void ThreadPool::submitTask(std::shared_ptr<Task> sp){
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
    }
    //如果有空余，把任务放到任务队列中
    taskQue_.emplace(sp);
    taskSize_++;
    //因为新放了任务，任务队列不为空，在notEmpty上进行通知，赶快分配线程执行任务
    notEmpty_.notify_all();
}

//开启线程池
void ThreadPool::start(int initThreadSize){
    //记录初始线程个数
    initThreadSize_ = initThreadSize;

    //创建线程对象
    for(int i = 0;i < initThreadSize_;i++) {
        std::unique_ptr<Thread> ptr = std::make_unique<Thread>(std::bind(&ThreadPool::threadFunc,this));
        //创建thread线程对象的时候，把线程函数给到thread线程对象
        //用于创建一个新的可调用对象，将ThreadPool 类的成员函数 threadFunc 和当前 ThreadPool 对象的实例（通过 this 指针）绑定在一起。
        //该对象用ThreadFunc接收，见Thread的构造函数。
        threads_.emplace_back(std::move(ptr));//注意！
    }

    //启动所有线程
    for(int i = 0;i < initThreadSize_;i++) {
        threads_[i] -> start();//需要去执行一个线程函数
    }
}

//定义线程函数 线程池的所有线程从任务队列里面消费任务
void ThreadPool::threadFunc(){
    // std::cout << "begin threadFunc"<< std::this_thread::get_id() << std::endl;
    // std::cout << "end threadFunc" << std::this_thread::get_id() << std::endl;
    for(;;) {
        std::shared_ptr<Task> task;
        {
            //先获取锁
            std::unique_lock<std::mutex> lock(taskQueMtx_);

            std::cout << "tid" << std::this_thread::get_id() << "尝试获取任务" << std::endl;
            //等待notEmpty条件
            notEmpty_.wait(lock,[&]() -> bool{return taskQue_.size() > 0;});

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
            task->run();
        }
    }

}
//线程方法实现
//线程构造
Thread::Thread(ThreadFunc func):func_(func){}

//线程析构
Thread::~Thread(){}
//启动线程
void Thread::start() {
    //创建一个线程来执行一个线程函数
    std::thread t(func_);// C++11来说，有线程对象t和线程函数func_
    t.detach();//设置分离对象
}


