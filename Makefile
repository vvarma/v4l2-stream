BUILD_DIR ?= local_build
INSTALL_DIR ?= local_build
BUILD_PROFILE ?= default
HOST_PROFILE ?= default
BUILD_TYPE ?= Release

.PHONY: all
all: clean build install 

.PHONY: clean 
clean:
	rm -rf $(BUILD_DIR)


.PHONY: build
build:
	conan install . -if=$(BUILD_DIR) -of=$(BUILD_DIR) --build=missing --profile:build=$(BUILD_PROFILE) --profile:host=$(HOST_PROFILE)
	cmake -S . -B $(BUILD_DIR) -G Ninja -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_TOOLCHAIN_FILE=$(BUILD_DIR)/conan_toolchain.cmake
	cmake --build $(BUILD_DIR) 

.PHONY: install
install:
	cmake --install ${BUILD_DIR} --prefix ${INSTALL_DIR}

.PHONY: dev
dev: build
	$(BUILD_DIR)/v4l2-stream/v4l2-stream


.PHONY: dev-docker-arm64
dev-docker-arm64:
	docker buildx build -o ./bin/armv8 --build-arg CC=aarch64-linux-gnu-gcc --build-arg CXX=aarch64-linux-gnu-g++ --build-arg HOST_PROFILE=armv8_debug --build-arg BUILD_TYPE=Debug . 

.PHONY: docker-arm64
docker-arm64:
	docker buildx build -o ./bin/armv8 --build-arg CC=aarch64-linux-gnu-gcc --build-arg CXX=aarch64-linux-gnu-g++ --build-arg HOST_PROFILE=armv8 . 
