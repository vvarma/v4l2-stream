project(pipeline)

add_library(${PROJECT_NAME} src/bcm2835-isp-decoder.cpp src/config.cpp src/decoders.cpp src/feedback.cpp src/loader.cpp src/pipeline.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME} PUBLIC v4l encoder PRIVATE spdlog::spdlog nlohmann_json::nlohmann_json)

add_executable(pipeline_test test/pipeline_test.cpp)
target_link_libraries(pipeline_test pipeline metrics spdlog::spdlog)
install(TARGETS pipeline_test)
