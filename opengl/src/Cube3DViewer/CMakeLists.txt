project(Cube3DViewer)
cmake_minimum_required(VERSION 3.0)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

option(USE_NETWORK_SOCKET "use network socket?" OFF)
if(USE_NETWORK_SOCKET)
    add_definitions("-DUSE_NETWORK_SOCKET")
    option(USE_PROTOBUF "use protobuf?" OFF)
    if(USE_PROTOBUF)
        add_definitions("-DUSE_PROTOBUF")
    endif()
endif()

if(WIN32)
else()
    set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -std=c++11")
    set(CMAKE_CXX_FLAGS_RELEASE "-W -Wall -O3 -std=c++11")
endif()

add_subdirectory(src/Cube3DViewer)

if(USE_NETWORK_SOCKET)
    add_subdirectory(src/test_client)
endif()
