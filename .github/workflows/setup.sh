#!/bin/bash


# We need clang 12
# https://apt.llvm.org/
deb http://apt.llvm.org/groovy/ llvm-toolchain-groovy-12 main
deb-src http://apt.llvm.org/groovy/ llvm-toolchain-groovy-12 main

sudo apt update

git submodule update --init --recursive

sudo apt install -y make build-essential bison flex texinfo nasm mtools wget \
                    unzip fuse libfuse-dev uuid-dev gcc binutils parted \
                    clang-12 llvm-12 llvm-ar
