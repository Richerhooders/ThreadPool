# ThreadPool
High-performance thread pool based on variable parameter template
1、基于可变参模板编程和引用折叠原理，实现线程池submitTask接口，支持任意任务函数和任意参数
的传递
2、使用future类型定制submitTask提交任务的返回值
3、使用map和queue容器管理线程对象和任务
4、基于条件变量condition_variable和互斥锁mutex实现任务提交线程和任务执行线程间的通信机制
5、支持fixed和cached模式的线程池定制
