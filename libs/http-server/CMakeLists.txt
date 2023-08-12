project(http-server)

add_library(http-server src/http-server.cpp src/request.cpp src/route.cpp src/responsewriter.cpp)
target_include_directories(http-server PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(http-server PRIVATE spdlog::spdlog fmt::fmt PUBLIC asio::asio)
