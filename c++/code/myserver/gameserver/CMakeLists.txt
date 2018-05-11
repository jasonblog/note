cmake_minimum_required(VERSION 3.6)
project(gameserver)
add_definitions(-D_DEBUG_ -DUSE_LOG4CPP -DUSE_MY_ASSERT)

set(SOURCE_FILES
        main.cpp
        inc/game_server.h
        inc/client_handle.h
        inc/message_factory.h
        inc/message_dispatcher.h
        src/server_handle.cpp
        inc/server_handle.h
        src/game_server.cpp
        src/client_handle.cpp
        src/message_factory.cpp
        src/message_dispatcher.cpp
        datamodule/inc/sceneobjmanager.h
        datamodule/inc/player.h
        datamodule/inc/playerdata.h
        datamodule/inc/playerbase.h
        datamodule/inc/playercity.h
        datamodule/src/player.cpp
        datamodule/src/playerbase.cpp
        datamodule/src/playerdata.cpp
        datamodule/src/playercity.cpp
        datamodule/src/sceneobjmanager.cpp
        logicmodule/inc/core_module.h
        logicmodule/inc/logic_module.h
        logicmodule/inc/module_manager.h
        logicmodule/inc/db_module.h
        logicmodule/src/module_manager.cpp
        logicmodule/src/core_module.cpp
        logicmodule/src/logic_module.cpp
        logicmodule/src/db_module.cpp
        metadata/inc/parameter_config.h
        metadata/src/parameter_config.cpp
        metadata/inc/parameter_key.h)

include_directories(
        /usr/local/include/
        ${CMAKE_SOURCE_DIR}/framework/base
        ${CMAKE_SOURCE_DIR}/framework/const
        ${CMAKE_SOURCE_DIR}/framework/event
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

add_executable(gameserver ${SOURCE_FILES})
target_link_libraries(gameserver framework pthread protobuf)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin/)