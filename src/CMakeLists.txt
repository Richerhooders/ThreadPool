# 查找当前目录中的所有源文件
aux_source_directory(. SRC_LIST)
#set(SRC_LIST threadpool.cc)
# 添加共享库
add_library(ThreadPool SHARED ${SRC_LIST})
target_link_libraries(ThreadPool pthread)