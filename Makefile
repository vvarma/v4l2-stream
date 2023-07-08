BUILD_DIR ?= local_build
BUILD_PROFILE ?= default
HOST_PROFILE ?= default

dev_build:
	conan install . --output-folder=$(BUILD_DIR) --build=missing --profile:build=$(BUILD_PROFILE) --profile:host=$(HOST_PROFILE)
	cmake -S . -B $(BUILD_DIR) -G Ninja -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake
	cmake --build $(BUILD_DIR) 
