cmake_minimum_required(VERSION 3.23)
project(http-server)

add_library(http-server src/http-server.cpp)
target_include_directories(http-server PUBLIC ${CMAKE_CURRENT_SOURCE_DIRECTORY}/include)
