# Building

BRUTAL can be built by using either LLVM or GNU toolchains.

In both cases you will need `nasm` and `make`.

On Arch Linux this can be done by doing `pacman -S make nasm`

> Run `git submodule update --init --recursive` to install the submodules before building

## With LLVM

Before trying to build BRUTAL with LLVM you need to install `lld` and `clang-17`.

On Arch Linux this can be done by doing `pacman -S lld clang`.

Build BRUTAL via the LLVM toolchain:

`make all`

> On some distros you might want to change the llvm version using `LLVM_VERSION=-17` or any other version.

## With GNU

In order to build BRUTAL with the GNU toolchain you will need to build the x86_64-elf binutils. This can be done by running the script in `build/toolchain/gnu/build.sh`.

After building the binutils you can build BRUTAL by running:

`make all TOOLCHAIN=gnu`

## On macOS

If you use [Homebrew package manager](https://brew.sh), you can obtain the dependencies by running `brew install qemu nasm xorriso llvm binutils wget pkg-config sdl2`.

LLVM on Homebrew is keg by default, you also need to set the following environment variables:

```sh
export PATH=$(brew --prefix llvm)/bin:$PATH
```

Then build BRUTAL by running:

`make all`

## Note About Windows

*Just switch to GNU+Linux*
