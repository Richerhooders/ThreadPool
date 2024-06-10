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

class Any {
public:
    Any() = default;
    ~Any() = default;
    Any(const Any&) = delete;
    Any& operator=(const Any&) = delete;
	Any(Any&&) = default;
	Any& operator=(Any&&) = default;

    //这个构造函数可以让Any接受其他任意的数据
    template<typename T>
    Any(T data):base_(std::make_unique<Derive<T>>(data)) {}

    //这个方法能把Any队形里面存储的data数据提取出来
    template<typename T>
    T cast_() {
        //怎么从base_找到它所指向的Derive对象，从它里面取出data成员变量
        // 基类指针 -》 派生类指针 RTTI
        Derive<T> *pd = dynamic_cast<Derive<T>*>(base_.get());
        if(pd == nullptr) {
            throw "type is unmatch";
        }
        return pd -> data_;
    }

private:
    //基类类型
    class Base {
    public:
        virtual ~Base() = default;
    };

    template <class T>
    class Derive: public Base {
    public:
        Derive(T data):data_(data) {}
        T data_; //保存了任意的其他类型
    };
private:
    //定义一个基类指针
    std::unique_ptr<Base> base_;
};

class Semaphore {
public:
    Semaphore(int limit = 0):resLimit_(limit) {};
    ~Semaphore() = default;

    //获取一个信号量资源
    void wait() {
        std::unique_lock<std::mutex> lock(mtx_);
        //等待信号量有资源，没有资源的话，会阻塞当前线程
        cond_.wait(lock,[&]()->bool{return resLimit_ > 0;});
        resLimit_--;
    }

    //增加一个信号量资源
    void post() {
        std::unique_lock<std::mutex> lock(mtx_);
        resLimit_++;
        cond_.notify_all();
    }
private:
    int resLimit_;
    std::mutex mtx_;
    std::condition_variable cond_;
};

class Task;//Task类型的前置声明
//实现接收提交到线程池的task任务执行完成后的返回值类型Result
class Result {
public:
    Result(std::shared_ptr<Task>task, bool isVaild = true);
    Result(const Result&) = delete;
    Result& operator=(const Result&) = delete;

    //问题一：setVal方法，获取任务执行完的返回值
    void setVal(Any any);
    //问题二：get方法，用户调用这个方法获取task的返回值
    Any get();
private:
    Any any_; //存储任务的返回值
    Semaphore sem_; // 线程通信的信号量
    std::shared_ptr<Task> task_;//指向对应获取返回值的任务对象，Result对象在Task就要在。
    std::atomic_bool isValid_;
};

//任务抽象基类
class Task {
public:
    Task();
    ~Task() = default;
    void exec();
    void setResult(Result* res);
    //用户可以自定义任意任务类型，从Task继承，重写run方法，实现自定义任务处理
    virtual Any run() = 0;
private:
    Result* result_;//不用用强智能指针，否则造成智能指针交叉引用问题会导致内存无法释放
    //Result的生命周期长于Task所以只需要用裸指针
};

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
    Thread(ThreadFunc func);

    //线程析构
    ~Thread();
    //启动线程
    void start();
    //获取线程id
    int getId() const;
private:
    ThreadFunc func_;
    static int generateId_;
    int threadId_; //保存线程id
};
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
    ThreadPool();

    //线程池析构
    ~ThreadPool();

    //设置线程池的工作模式
    void setMode(PoolMode mode);

    //设置task任务队列上限阈值
    void setTaskQueMaxThreshHold(int threshhold);

    //设置线程数量上限阈值
    void setThreadSizeThreshHold(int threshhold);

    //给线程池提交任务
    Result submitTask(std::shared_ptr<Task> sp);

    //开启线程池
    void start(int initThreadSize = std::thread::hardware_concurrency());

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool &operator = (const ThreadPool&) = delete;
private:
    //定义线程函数
    void threadFunc(int threadId);

    //检查pool 运行状态
    bool checkRunningState() const;
private:
    // std::vector<std::unique_ptr<Thread>> threads_; //线程列表
    std::unordered_map<int,std::unique_ptr<Thread>> threads_;//线程列表
    size_t initThreadSize_; //初始线程数量
    std::atomic_int curThreadSize_;//记录当前线程池里面现成数量
    std::atomic_int idleThreadSize_;//记录空闲线程的数量
    int threadSizeThresdHold_; //现成数量上限阈值

    std::queue<std::shared_ptr<Task>> taskQue_; //任务队列 不可能要求用户传入一个生命周期足够长的对象
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