project(v4l)

add_library(${PROJECT_NAME} src/v4l_bridge.cpp src/v4l_caps.cpp src/v4l_capture.cpp src/v4l_device.cpp src/v4l_exception.cpp src/v4l_framesize.cpp src/v4l_output.cpp src/v4l_stream.cpp src/util.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME} PRIVATE fmt::fmt spdlog::spdlog)
