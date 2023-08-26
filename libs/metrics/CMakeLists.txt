project(metrics)

add_library(${PROJECT_NAME} src/metrics.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME} PUBLIC nlohmann_json::nlohmann_json)
