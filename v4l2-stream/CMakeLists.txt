project(v4l2-stream)

add_executable(${PROJECT_NAME} src/main.cpp src/stream.cpp)
target_link_libraries(${PROJECT_NAME} http-server pipeline v4l spdlog::spdlog structopt::structopt)
install(TARGETS ${PROJECT_NAME})
