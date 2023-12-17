BUILD_DIR ?= local_build
INSTALL_DIR ?= local_build
BUILD_PROFILE ?= default
HOST_PROFILE ?= default
BUILD_TYPE ?= Release
BUILD_VERSION ?= 0.0.0

DOCKER_OPTS := --secret=id=netrc,src=${HOME}/.netrc --build-arg BUILD_VERSION=$(BUILD_VERSION)
DOCKER_OUT := ./bin

CMAKE_BUILD_OPTS := -j4 

.PHONY: all
all: clean build install 

.PHONY: clean 
clean:
	rm -rf $(BUILD_DIR)

.PHONY: build
build:
	conan install . -if=$(BUILD_DIR) -of=$(BUILD_DIR) --build=missing --profile:build=$(BUILD_PROFILE) --profile:host=$(HOST_PROFILE)
	cmake -S . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_TOOLCHAIN_FILE=$(BUILD_DIR)/conan_toolchain.cmake -DENABLE_TESTS=OFF -DBUILD_VERSION=$(BUILD_VERSION)
	cmake --build $(BUILD_DIR) ${CMAKE_BUILD_OPTS} 

.PHONY: build-dpkg
build-dpkg: build
	cd $(BUILD_DIR) && cpack -G DEB

.PHONY: install
install:
	cmake --install ${BUILD_DIR} --prefix ${INSTALL_DIR}

.PHONY: dev
dev: BUILD_TYPE = Debug
dev: HOST_PROFILE = debug
dev: build install
	$(INSTALL_DIR)/bin/v4l2-stream

.PHONY: docker-build
docker-build:
	docker buildx build -o $(DOCKER_OUT) $(DOCKER_OPTS) --no-cache .

.PHONY: docker-arm64
docker-arm64: DOCKER_OPTS += --build-arg CC=aarch64-linux-gnu-gcc --build-arg CXX=aarch64-linux-gnu-g++ --build-arg HOST_PROFILE=armv8 --build-arg=ARCH=armv8 --build-arg --progress=plain
docker-arm64: docker-build

.PHONY: docker-arm
docker-arm: DOCKER_OPTS += --build-arg CC=arm-linux-gnueabihf-gcc --build-arg CXX=arm-linux-gnueabihf-g++ --build-arg HOST_PROFILE=arm --build-arg ARCH=armv7hf --build-arg --progress=plain
docker-arm: docker-build
