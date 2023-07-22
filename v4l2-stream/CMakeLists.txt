cmake_minimum_required(VERSION 3.23)
project(v4l2-stream)

add_executable(${PROJECT_NAME} src/main.cpp src/stream.cpp)
target_link_libraries(${PROJECT_NAME} http-server pipeline v4l)
