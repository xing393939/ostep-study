# https://www.hahack.com/codes/cmake/
cmake_minimum_required(VERSION 3.10)
project(untitled)
set(CMAKE_CXX_STANDARD 14)

add_executable(test_mem code/test_mem.c)
add_executable(t1 threads-intro/t1.c include/common.h include/common_threads.h)
TARGET_LINK_LIBRARIES(t1 pthread)

add_executable(thread_create threads-api/thread_create.c)
TARGET_LINK_LIBRARIES(thread_create pthread)