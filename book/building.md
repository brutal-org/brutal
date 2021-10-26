# Building

BRUTAL can be built by using either LLVM or GNU toolchains.

In both cases you will need `xorriso`, `nasm` and `make`.

On Arch Linux this can be done by doing `pacman -S xorriso make nasm`

> Run `git submodule update --init --recursive` to install the submodules before building

## With LLVM

Before trying to build BRUTAL with LLVM you need to install `lld` and `clang-12`.

On Arch Linux this can be done by doing `pacman -S lld clang`.

Build BRUTAL via the LLVM toolchain:

`make all`

> On some distros you might want to change the llvm version using `LLVM_VERSION=-12` or any other version.

## With GNU

In order to build BRUTAL with the GNU toolchain you will need to build the x86_64-elf binutils. This can be done by running the script in `build/toolchain/gnu/build.sh`.

After building the binutils you can build BRUTAL by running:

`make all CONFIG_TOOLCHAIN=gnu`

## Note About Windows

*Just switch to GNU+Linux*
