cmake_minimum_required(VERSION 3.6)
set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)

project(server)
add_compile_options(-Wall -g -pipe -std=c++14)
add_subdirectory(framework)
add_subdirectory(gateserver)
add_subdirectory(proxyserver)
add_subdirectory(dbserver)
add_subdirectory(gameserver)



