# Build Instructions

BRUTAL can be built by using either LLVM or GNU toolchains.

In both cases you will need ``xorriso``, ``nasm`` and ``make``. On Arch Linux this can be done by doing ``pacman -S xorriso make nasm``.

_NOTE: Run ``git submodule update --init --recursive`` to install the submodules before building_

## With LLVM

Before trying to build BRUTAL with LLVM you need to install ``lld`` and ``clang-12``. On Arch Linux this can be done by doing ``pacman -S lld clang``.

Build BRUTAL via the LLVM toolchain:

``make run CONFIG_TOOLCHAIN=llvm``

## With GNU

In order to build BRUTAL with the GNU toolchain you will need to build the x86_64-elf binutils. This can be done by running the script in ``meta/toolchain/gnu/build.sh``.

After building the binutils you can build BRUTAL by running:

``make run CONFIG_TOOLCHAIN=gnu``
