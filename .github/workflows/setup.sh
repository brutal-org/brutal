#!/bin/bash

sudo apt update
git submodule update --init --recursive

sudo apt install -y make build-essential bison flex texinfo nasm mtools wget \
                    unzip fuse libfuse-dev uuid-dev gcc binutils parted
