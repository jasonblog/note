cmake_minimum_required(VERSION 3.8)
project(threadpool)
set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)
add_compile_options(-Wall -g -pipe -std=c++14)

set(SOURCE_FILES
        CMakeLists.txt
        main.cpp
        threadpool.h)

add_executable(threadpool ${SOURCE_FILES})
target_link_libraries(threadpool pthread)