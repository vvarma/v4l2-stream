project(pipeline)

add_library(${PROJECT_NAME} src/pipeline.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME} PUBLIC v4l encoder PRIVATE libev::libev)
