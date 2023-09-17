project(pipeline)

add_library(rpi-algos OBJECT 
  src/rpi-agc.cpp 
  src/rpi-awb.cpp 
  src/rpi-denoise.cpp 
  src/rpi-sharpen.cpp
  )
target_link_libraries(rpi-algos PRIVATE v4l spdlog::spdlog)

add_library(rpi-decoders OBJECT src/decoders/bcm2835-isp-decoder.cpp)
target_link_libraries(rpi-decoders PRIVATE v4l spdlog::spdlog)

add_library(${PROJECT_NAME} 
  src/algorithm.cpp 
  src/config.cpp 
  src/decoder.cpp
  src/feedback.cpp 
  src/loader.cpp 
  src/pipeline.cpp
  )
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME} PUBLIC v4l encoder PRIVATE spdlog::spdlog nlohmann_json::nlohmann_json)

add_executable(pipeline_test test/pipeline_test.cpp)
target_link_libraries(pipeline_test pipeline metrics spdlog::spdlog)
install(TARGETS pipeline_test)
