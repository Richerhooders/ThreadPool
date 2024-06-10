#ifndef THREADPOOL_H
#define THREADPOOL_H
#include <iostream>
#include <vector>
#include <queue>
#include <memory>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <thread>
#include <unordered_map>
#include <future>

const int TASK_MAX_THRESHHOLD = 2;
const int THREAD_MAX_THRESHHOLD = 100;
const int THREAD_MAX_IDLE_TIME = 60; // 单位：秒

//线程池支持的模式
enum class PoolMode {
    MODE_FIXED, //固定数量的线程
    MODE_CACHED, //线程数量可动态增长
};

//线程类型
class Thread {
public:
    //线程构造函数类型
    using ThreadFunc = std::function<void(int)>;//用来接受函数对象

    //线程构造
    Thread(ThreadFunc func):func_(func),threadId_(generateId_++){}

    //线程析构
    ~Thread() = default;

    //启动线程
    void start() {
        //创建一个线程来执行一个线程函数
        std::thread t(func_,threadId_);// C++11来说，有线程对象t和线程函数func_
        t.detach();//设置分离对象
    }

    //获取线程id
    int getId() const {
        return threadId_;
    }

private:
    ThreadFunc func_;
    static int generateId_;
    int threadId_; //保存线程id
};

int Thread::generateId_ = 0;
/*
example:
ThreadPool pool;
pool.start(4);

class MyTask:public Task{
public:
    void run() {
        //线程代码
    }
pool.submitTask(std::make_shared<MyTask>());
}
*/
//线程池类型
class ThreadPool{
public:
    //线程池构造
    ThreadPool():initThreadSize_(4),
                 taskSize_(0),
                 taskQueMaxThreshHold_(TASK_MAX_THRESHHOLD),
                 threadSizeThresdHold_(THREAD_MAX_THRESHHOLD),
                 curThreadSize_(0),
                 poolMode_(PoolMode::MODE_FIXED),
                 isPoolRunning_(false),
                 idleThreadSize_(0)
                {}

    //线程池析构
    ~ThreadPool(){
        isPoolRunning_ = false;
        // notEmpty_.notify_all();

        //等待线程池里面所有的线程返回 有两种状态：阻塞 正在执行任务中,第三情况：可能发生死锁，线程刚进入循环处于拿锁状态
        std::unique_lock<std::mutex> lock(taskQueMtx_);
        notEmpty_.notify_all(); //避免死锁，比如线程池线程先拿到锁进入的等待的时候，还有机会去唤醒。
        exitCond_.wait(lock,[&]() -> bool{return threads_.size() == 0;});//主线程(用户线程阻塞在这里等待线程池中的线程回收)
    }

    //设置线程池的工作模式
    void setMode(PoolMode mode){
        if(checkRunningState()) {
            return; 
        }
        poolMode_ = mode;
    }

    //设置task任务队列上限阈值
    void setTaskQueMaxThreshHold(int threshhold){
        if(checkRunningState()) {
            return; 
        }
        taskQueMaxThreshHold_ = threshhold;
    }

    //设置线程数量上限阈值
    void setThreadSizeThreshHold(int threshhold){
        if(checkRunningState()) {
            return;
        }
        if(poolMode_ == PoolMode::MODE_CACHED) {
            threadSizeThresdHold_ = threshhold;
        }
    }

    //给线程池提交任务
    //使用可变参模板编程，让submitTask可以接收任意任务函数和任意数量的参数
    //返回值需要一个future<>,推导出来返回值类型,然后实例化future
    template<typename Func,typename... Args>
    auto submitTask(Func&& func,Args&&... args) -> std::future<decltype(func(args...))> {
        //打包任务放入任务队列
        using RType = decltype(func(args...)); //推导出来的是类型
        auto task = std::make_shared<std::packaged_task<RType()>>(
            std::bind(std::forward<Func>(func),std::forward<Args>(args)...)); 
        std::future<RType> result = task -> get_future();
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
            auto task = std::make_shared<std::packaged_task<RType()>>(
                []() -> RType {return RType();});
            (*task)();//执行这个任务，不然不执行获取返回值会崩溃
            return  task -> get_future();
        }
        //如果有空余，把任务放到任务队列中
        // taskQue_.emplace(sp);  using Task = std::function<void()>;//不能确定返回值的类型
        taskQue_.emplace([task]() {(*task)();}); //增加一个中间层返回值是void不带参数的一个lamda表达式将实际要执行的任务封装起来

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

        return result;
        // return task -> getResult();
    }

    //开启线程池
    void start(int initThreadSize = std::thread::hardware_concurrency()){
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

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool &operator = (const ThreadPool&) = delete;
private:
    //定义线程函数
    void threadFunc(int threadId){ //线程函数返回，相应的线程也就结束了
        auto lastTime = std::chrono::high_resolution_clock().now();
        //所有任务必须执行完成，线程池才可以回收所有资源
        for(;;){
            Task task;//自己创建的，生命周期自己负责，无需智能指针
            {
                //先获取锁
                std::unique_lock<std::mutex> lock(taskQueMtx_);

                std::cout << "tid" << std::this_thread::get_id() << "尝试获取任务" << std::endl;

                //有任务不会回收资源，必须任务执行完成任务队列为空
                while(taskQue_.size() == 0) {
                    //线程池要结束，回收线程资源
                    if(!isPoolRunning_) { //回收资源
                        threads_.erase(threadId);
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
                                threads_.erase(threadId);
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
                task(); // 执行function<void()>
            }
            idleThreadSize_++;
            lastTime = std::chrono::high_resolution_clock().now(); //更新线程执行完任务的时间
        }
    }

    //检查pool 运行状态
    bool checkRunningState() const{
        return isPoolRunning_;
    }
private:
    // std::vector<std::unique_ptr<Thread>> threads_; //线程列表
    std::unordered_map<int,std::unique_ptr<Thread>> threads_;//线程列表
    size_t initThreadSize_; //初始线程数量
    std::atomic_int curThreadSize_;//记录当前线程池里面现成数量
    std::atomic_int idleThreadSize_;//记录空闲线程的数量
    int threadSizeThresdHold_; //现成数量上限阈值

    //Task任务 -》 函数对象
    using Task = std::function<void()>;
    std::queue<Task> taskQue_;  //任务队列
    std::atomic_uint taskSize_; //任务的数量
    int taskQueMaxThreshHold_;  //任务队列数量上限阈值

    std::mutex taskQueMtx_; // 保证任务队列的线程安全
    std::condition_variable notFull_; //任务队列不满
    std::condition_variable notEmpty_; //任务队列不空
    std::condition_variable exitCond_; //等待线程资源全部回收

    PoolMode poolMode_; //当前线程池的工作模式
    //表示当前线程池的启动状态
    std::atomic_bool isPoolRunning_; // 可能在多个线程中，使用原子类型
};

#endif