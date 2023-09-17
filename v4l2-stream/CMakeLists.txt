project(v4l2-stream)

add_executable(${PROJECT_NAME} src/main.cpp src/ctrls.cpp src/metrics.cpp src/selection.cpp src/stream.cpp)
target_link_libraries(${PROJECT_NAME} http-server pipeline v4l rpi-algos rpi-decoders spdlog::spdlog structopt::structopt nlohmann_json::nlohmann_json)
install(TARGETS ${PROJECT_NAME})
