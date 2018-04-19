add_executable(test_client
    client.cpp
    SocketPoseClient.cpp
    ../../thirdparty/TinyTcp/src/TinyTcp/TinyTcpClient.cpp
    ../../thirdparty/TinyTcp/src/TinyTcp/SocketUtil.cpp
)

target_include_directories(test_client PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../../thirdparty/TinyTcp/src/TinyTcp")
target_link_libraries(test_client pthread)