cmake_minimum_required(VERSION 3.6)
project(myserver)

#libevent 多线程开启-DEVENT_THREAD_SAFE
add_definitions(-D_DEBUG_ -DUSE_MY_ASSERT -DLOG_THREAD_SAFE)
#add_definitions(-D_DEBUG_ -DUSE_MY_ASSERT -DEVENT_THREAD_SAFE -DLOG_THREAD_SAFE)

set(FRAMEWORK
        log/log.h
        log/log.cpp
        base/base.h
        base/object.h
        base/performance.h
        base/server_tool.h
        base/base.cpp
        base/performance.cpp
        base/server_tool.cpp
        base/my_assert.h
        base/my_macro.h
        base/my_macro.h
        base/object_manager.cpp
        base/object_manager.h
        const/common_def.h
        const/err_code.h
        const/dblogin_type.h
        json/json.h
        json/json.cpp
        json/config.h
        json/config.cpp
        message/message.pb.h
        message/message.pb.cc
        message/player.pb.h
        message/player.pb.cc
        message/dbmessage.pb.h
        message/dbmessage.pb.cc
        message/common.pb.h
        message/common.pb.cc
        net/statistics.h
        net/runflag.h
        net/runflag.cpp
        net/statistics.cpp
        net/ccrypto.h
        net/ccrypto.cpp
        net/client_comm_engine.h
        net/server_comm_engine.h
        net/message_interface.h
        net/message_interface.cpp
        net/server_comm_engine.cpp
        net/client_comm_engine.cpp
        net/acceptor.h
        net/acceptor.cpp
        net/connector.h
        net/connector.cpp
        net/event_reactor.h
        net/event_reactor.cpp
        net/listener.h
        net/listener.cpp
        net/net_addr.h
        net/net_inc.h
        net/net_work.h
        net/net_work.cpp
        net/network_interface.h
        net/system_signal.h
        net/system_signal.cpp
        net/socket.h
        net/socket.cpp
        net/buffev_interface.cpp
        net/buffev_interface.h
        net/timer_event.h
        net/timer_event.cpp
        net/byte_buff.cpp
        net/byte_buff.h
        mem/share_mem.h
        mem/shm.h/
        mem/share_mem.cpp
        mem/shm.cpp
        mem/code_queue.h
        mem/code_queue.cpp
        mem/mem_pool.h
        mem/mempool_safty.h
        thread/mythread.h
        thread/mythread.cpp
        thread/thread_pool.h
        timer/clock.cpp
        timer/clock.h
        timer/timer.h
        timer/timer.cpp)

include_directories(
        /usr/include
        /usr/local/include
        /usr/local/include/rapidjson/include
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
        /usr/local/lib
        ${CMAKE_SOURCE_DIR}/lib
)

add_library(framework ${FRAMEWORK})
target_link_libraries(framework pthread event event_pthreads protobuf log4cpp crypto)
set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/lib)
