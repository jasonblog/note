find_package(glfw3 REQUIRED)

set(FILES
    main.cpp
    GLUtil.cpp
)

if(USE_NETWORK_SOCKET)
    list(APPEND FILES
        SocketPoseService.cpp
        ../../thirdparty/TinyTcp/src/TinyTcp/TinyTcpServer.cpp
        ../../thirdparty/TinyTcp/src/TinyTcp/SocketUtil.cpp
    )
    list(APPEND INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/../../thirdparty/TinyTcp/src/TinyTcp")

else()
    list(APPEND FILES
        FakePoseGenerator.cpp
    )
endif()

if(USE_PROTOBUF)
    find_package(Protobuf REQUIRED)
    PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS pose.proto)
    list(APPEND FILES ${PROTO_SRCS})
    list(APPEND INCLUDE_DIRS ${PROTOBUF_INCLUDE_DIRS})
    list(APPEND LIBRARIES ${PROTOBUF_LIBRARIES})
endif()

add_executable(cube3dviewer ${FILES})

target_include_directories(cube3dviewer PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../../thirdparty/TinyTcp/src/TinyTcp" ${INCLUDE_DIRS})
target_link_libraries(cube3dviewer GL GLEW glfw pthread ${LIBRARIES})
