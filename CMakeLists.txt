# https://www.hahack.com/codes/cmake/
cmake_minimum_required(VERSION 3.10)
project(untitled)
set(CMAKE_CXX_STANDARD 14)

add_executable(test_bpf code/test_bpf.c)
add_executable(test_mem code/test_mem.c)
add_executable(test_asm code/test_asm.c)
add_executable(pointers code/pointers.c)
add_executable(cycle_linked_list code/cycle_linked_list.c)
add_executable(t1 threads-intro/t1.c include/common.h include/common_threads.h)
TARGET_LINK_LIBRARIES(t1 pthread)

add_executable(thread_create threads-api/thread_create.c)
TARGET_LINK_LIBRARIES(thread_create pthread)

add_executable(join_ threads-cv/join.c include/common.h include/common_threads.h)
TARGET_LINK_LIBRARIES(join_ pthread)

add_executable(join_modular threads-cv/join_modular.c include/common.h include/common_threads.h)
TARGET_LINK_LIBRARIES(join_modular pthread)