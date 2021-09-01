#!/bin/bash

# We need clang 12
# https://apt.llvm.org/
bash -c "$(wget -O - https://apt.llvm.org/llvm.sh)"

sudo apt update

sudo add-apt-repository ppa:ubuntu-toolchain-r/test
sudo apt install -y gcc-11 g++-11
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-11 100 --slave /usr/bin/g++ g++ /usr/bin/g++-11 --slave /usr/bin/gcov gcov /usr/bin/gcov-11

git submodule update --init --recursive

sudo apt install -y make build-essential bison flex texinfo nasm mtools wget \
                    unzip fuse libfuse-dev uuid-dev gcc binutils parted \
                    clang-12 llvm-12 lld
