#include "threadpool.h"
#include <iostream>
#include <chrono>

class MyTask:public Task{
public:
    MyTask(int begin,int end):begin_(begin),end_(end) {

    }
    //问题一：如何设计run函数的返回值，可以表示任意类型
    Any run() {
        std::cout << "tid" << std::this_thread::get_id() << "begin" << std::endl;
        // std::this_thread::sleep_for(std::chrono::seconds(3));
        unsigned long long sum = 0;
        for(int i = begin_;i <= end_;i++) {
            sum += i;
        }
        std::cout << "tid" << std::this_thread::get_id() << "end" << std::endl;
        return sum;
    }
private:
    int begin_;
    int end_;
};

int main() {
    {

        ThreadPool pool;
        // pool.setMode(PoolMode::MODE_CACHED);
        pool.setMode(PoolMode::MODE_FIXED);
        pool.start(2);

        Result res1 = pool.submitTask(std::make_shared<MyTask>(1,100000000));
        Result res2 = pool.submitTask(std::make_shared<MyTask>(100000001,200000000));
        pool.submitTask(std::make_shared<MyTask>(100000001,200000000));
        pool.submitTask(std::make_shared<MyTask>(100000001,200000000));
        pool.submitTask(std::make_shared<MyTask>(100000001,200000000));

        unsigned long long sum1 = res1.get().cast_<unsigned long long>();//阻塞等待任务的执行结果
        std::cout << sum1 << std::endl;

    }//这里Result对象也要析构！！在vs下，条件变量的析构会释放相应资源
    //Result 析构 -> Result中的Semphore析构 -> Semphore中的互斥量mutex和条件变量cond_析构,但是g++库中没有释放资源

    // std::cout << sum1 << std::endl;
    std::cout << "main over" << std::endl;
    getchar();
  /*  //问题：threadPool对象析构以后，怎么样把线程池相关的线程资源全部回收？
    ThreadPool pool;
    //用户自己设置线程池的工作模式
    pool.setMode(PoolMode::MODE_CACHED);
    pool.start(4);
    //如何设计Result机制
    Result res1 = pool.submitTask(std::make_shared<MyTask>(1,100000000));
    Result res2 = pool.submitTask(std::make_shared<MyTask>(100000001,200000000));
    Result res3 = pool.submitTask(std::make_shared<MyTask>(200000001,300000000));
    pool.submitTask(std::make_shared<MyTask>(200000001,300000000));

    pool.submitTask(std::make_shared<MyTask>(200000001,300000000));
    pool.submitTask(std::make_shared<MyTask>(200000001,300000000));

    //get返回了一个Any类型，怎么转成具体类型
    unsigned long long sum1 = res1.get().cast_<unsigned long long>();
    unsigned long long sum2 = res2.get().cast_<unsigned long long>();
    unsigned long long sum3 = res3.get().cast_<unsigned long long>();
    // unsigned long long sum = 0;
    // for(int i = 1;i <= 300000000;i++) {
    //         sum += i;
    // }
    //Master - Slave现成模型
    //Master用来分解任务，然后给各个slave现成分配任务，等待各个Slave线程执行完任务，返回结果
    //Master线程合并各个任务结果，输出
    std::cout << sum1 + sum2 + sum3 << std::endl;
    // std::cout << sum << std::endl;
    */
}