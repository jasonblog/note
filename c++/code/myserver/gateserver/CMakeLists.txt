cmake_minimum_required(VERSION 3.6)
project(gateserver)
add_compile_options(-Wall -g -pipe -O0 -std=c++14)
add_definitions(-D_DEBUG_ -DMEDIUM_SIZE -DUSE_MY_ASSERT -DLINUX)
#add_definitions(-DMEDIUM_SIZE -D_TGW_ -DUSE_MY_ASSERT -DLINUX)

set(SOURCE_FILES
        main.cpp
        inc/gate_def.h
        inc/gate_ctrl.h
        src/gate_ctrl.cpp
        inc/c2s_handle.h
        src/c2s_handle.cpp
        src/s2c_handle.cpp
        inc/s2c_handle.h)

include_directories(
        /usr/include
        /usr/local/include/
        ${CMAKE_SOURCE_DIR}/framework/base
        ${CMAKE_SOURCE_DIR}/framework/const
        ${CMAKE_SOURCE_DIR}/framework/json
        ${CMAKE_SOURCE_DIR}/framework/log
        ${CMAKE_SOURCE_DIR}/framework/mem
        ${CMAKE_SOURCE_DIR}/framework/message
        ${CMAKE_SOURCE_DIR}/framework/net
        ${CMAKE_SOURCE_DIR}/framework/thread
        ${CMAKE_SOURCE_DIR}/framework/timer
)

link_directories(
        /usr/local/lib/
        ${CMAKE_SOURCE_DIR}/lib
)

add_executable(gateserver ${SOURCE_FILES})
target_link_libraries(gateserver framework pthread event event_pthreads protobuf log4cpp crypto)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)