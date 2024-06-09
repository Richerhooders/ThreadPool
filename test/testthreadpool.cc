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
        int sum = 0;
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
    ThreadPool pool;
    pool.start(4);
    //如何设计Result机制
    Result res = pool.submitTask(std::make_shared<MyTask>(1,100000000));
    res.get().cast_<int>();//get返回了一个Any类型，怎么转成具体类型

    getchar();
}