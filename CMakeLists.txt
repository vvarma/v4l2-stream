cmake_minimum_required(VERSION 3.23)
project(v4l2-stream)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DASIO_ENABLE_HANDLER_TRACKING")
set(CMAKE_CXX_FLAGS "-fcoroutines")
include(cmake/dependencies.cmake)
add_subdirectory(libs)
add_subdirectory(v4l2-stream)
 

