project(encoder)

add_library(${PROJECT_NAME} src/encoded_parts.cpp src/mjpeg_encoder.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME} PUBLIC v4l coro PRIVATE spdlog::spdlog)

