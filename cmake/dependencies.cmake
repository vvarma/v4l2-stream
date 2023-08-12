find_package(spdlog REQUIRED)
find_package(fmt REQUIRED)
find_package(libev REQUIRED)
find_package(structopt REQUIRED)
find_package(nlohmann_json REQUIRED)

include(FetchContent)

FetchContent_Declare(http-server GIT_REPOSITORY https://github.com/vvarma/http-server.git GIT_TAG fix/stream)
FetchContent_MakeAvailable(http-server)
