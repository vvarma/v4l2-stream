# syntax = docker/dockerfile:1.4.0
FROM debian:bullseye AS base
RUN apt-get update && apt-get install -y --no-install-recommends \
  build-essential \
  crossbuild-essential-arm64 \
  crossbuild-essential-armhf \
  python3 \
  python3-setuptools \
  python3-wheel \
  python3-pip \
  python3-venv \
  ccache \
  wget \
  ninja-build \
  git \
  && rm -rf /var/lib/apt/lists/*
RUN mkdir /cmake/ && cd /cmake/ \
  && wget https://github.com/Kitware/CMake/releases/download/v3.28.1/cmake-3.28.1-linux-x86_64.tar.gz \
  && tar -xzvf cmake-3.28.1-linux-x86_64.tar.gz
ENV PATH=$PATH:/cmake/cmake-3.28.1-linux-x86_64/bin
RUN cmake --version

ENV VIRTUAL_ENV=/venv
RUN python3 -m venv $VIRTUAL_ENV
ENV PATH="$VIRTUAL_ENV/bin:$PATH"
RUN pip install -U pip "conan<2" \ 
  && conan profile new --detect default \ 
  && conan profile update settings.compiler.libcxx=libstdc++11 default
ENV target_host=aarch64-linux-gnu
RUN <<EOF cat >> ~/.conan/profiles/armv8
include(default)
[settings]
arch=armv8
arch_build=armv8
[env]
CHOST=$target_host
AR=$target_host-ar
AS=$target_host-as
RANLIB=$target_host-ranlib
CC=$target_host-gcc
CXX=$target_host-g++
STRIP=$target_host-strip
RC=$target_host-windres
CONAN_CMAKE_SYSTEM_PROCESSOR=armv8
EOF
RUN <<EOF cat >> ~/.conan/profiles/armv8_debug
include(armv8)
[settings]
build_type=Debug
EOF
ENV target_host=arm-linux-gnueabihf
RUN <<EOF cat >> ~/.conan/profiles/arm
include(default)
[settings]
arch=armv7hf
[env]
CHOST=$target_host
AR=$target_host-ar
AS=$target_host-as
RANLIB=$target_host-ranlib
CC=$target_host-gcc
CXX=$target_host-g++
STRIP=$target_host-strip
RC=$target_host-windres
CONAN_CMAKE_SYSTEM_PROCESSOR=armv7hf
EOF

FROM base AS build

COPY . /code
WORKDIR /code
ARG HOST_PROFILE=default
ARG BUILD_TYPE=Release
ARG BUILD_DIR=local_build
ARG CC=gcc
ARG CXX=g++
ARG ARCH=amd64
ARG BUILD_VERSION=0.0.0

ENV HOST_PROFILE=$HOST_PROFILE
ENV BUILD_TYPE=$BUILD_TYPE
ENV CC=$CC
ENV CXX=$CXX
ENV BUILD_DIR=$BUILD_DIR
ENV ARCH=$ARCH
ENV BUILD_VERSION=$BUILD_VERSION

RUN --mount=type=cache,target=/root/.conan/data make build-dpkg

FROM scratch

COPY --from=build /code/local_build/*.deb /
