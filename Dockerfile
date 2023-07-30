# syntax = docker/dockerfile:1.4.0
FROM debian:bookworm AS base
RUN apt-get update && apt-get install -y --no-install-recommends \
  build-essential \
  crossbuild-essential-arm64 \
  python3 \
  python3-setuptools \
  python3-wheel \
  python3-pip \
  python3-venv \
  cmake\
  ninja-build \
  && rm -rf /var/lib/apt/lists/*
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

FROM base AS build

COPY . /code
WORKDIR /code
ARG HOST_PROFILE=default
ARG BUILD_TYPE=Release
ARG INSTALL_DIR=bin
ARG CC=gcc
ARG CXX=g++

ENV HOST_PROFILE=$HOST_PROFILE
ENV BUILD_TYPE=$BUILD_TYPE
ENV INSTALL_DIR=$INSTALL_DIR
ENV CC=$CC
ENV CXX=$CXX

RUN --mount=type=cache,target=/root/.conan/data make

FROM scratch

COPY --from=build /code/bin/* /
