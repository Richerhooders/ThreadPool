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
        std::this_thread::sleep_for(std::chrono::seconds(5));
        std::cout << "tid" << std::this_thread::get_id() << "end" << std::endl;
    }
private:
    int begin_;
    int end_;
};

int main() {
    ThreadPool pool;
    pool.start(4);
    //如何设计Result机制
    Result res = pool.submitTask(std::make_shared<MyTask>());
    pool.submitTask(std::make_shared<MyTask>());
    pool.submitTask(std::make_shared<MyTask>());
    pool.submitTask(std::make_shared<MyTask>());

    getchar();
}