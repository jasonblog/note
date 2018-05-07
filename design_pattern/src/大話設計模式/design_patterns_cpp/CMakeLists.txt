cmake_minimum_required(VERSION 3.0)
project(skeleton VERSION 0.0.1)

#
# Build Options
#
option( BUILD_SHARED   "Build shared library."              ON )
option( BUILD_EXAMPLES "Build examples applications."       ON )
option( BUILD_TESTS    "Build all available test suites."   ON )
option( BUILD_SSL      "Build secure socket layer support."  ON )
option( SanitizeAddress "compile with -fsanitize=address"    OFF )

#
# Configuration
#
include( "${CMAKE_SOURCE_DIR}/cmake/configuration.cmake" )
include(GNUInstallDirs)
if(SanitizeAddress)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
endif()
#
# Dependencies
#
set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules" )

include_directories( ${CMAKE_SOURCE_DIR} )

include_directories( ${INCLUDE_DIR} )

add_subdirectory(src)

if ( BUILD_TESTS )
    enable_testing( )
    add_subdirectory( "${CMAKE_SOURCE_DIR}/test")
endif ( )
