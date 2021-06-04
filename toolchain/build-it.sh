#!/bin/bash
set -e

# ----- Configs -------------------------------------------------------------- #

BINUTILS_VERSION=2.36
BINUTILS_DIRECTORY="binutils-$BINUTILS_VERSION"
BINUTILS_FILENAME="$BINUTILS_DIRECTORY.tar.gz"
BINUTILS_URL="http://ftp.gnu.org/gnu/binutils/$BINUTILS_FILENAME"

GCC_VERSION=11.1
GCC_DIRECTORY="gcc-11.1.0-RC-20210423"
GCC_FILENAME="gcc-11.1.0-RC-20210423.tar.gz"
GCC_URL="https://gcc.gnu.org/pub/gcc/snapshots/11.1.0-RC-20210423/gcc-11.1.0-RC-20210423.tar.gz"

# ---------------------------------------------------------------------------- #

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

PREFIX="$DIR/local"

if [ -e "$PREFIX/build-ok" ]; then
    echo "The toolchain is built and ready :)"
    exit 0
fi

cd "$DIR"

mkdir -p tarballs

source "$DIR/use-it.sh"

# Download and unpack GCC and binutils
# ---------------------------------------------------------------------------- #

pushd tarballs
    if [ ! -e "$BINUTILS_FILENAME" ]; then
        wget $WGETFLAGS "$BINUTILS_URL"
    else
        echo "Skipped downloading binutils"
    fi

    if [ ! -e "$GCC_FILENAME" ]; then
        wget $WGETFLAGS "$GCC_URL"
    else
        echo "Skipped downloading gcc"
    fi

    if [ ! -d "$BINUTILS_DIRECTORY" ]; then
        echo "Extracting binutils..."
        tar -xf "$BINUTILS_FILENAME"
    else
        echo "Skipped extracting binutils"
    fi

    if [ ! -d "$GCC_DIRECTORY" ]; then
        echo "Extracting gcc..."
        tar -xf "$GCC_FILENAME"
    else
        echo "Skipped extracting gcc"
    fi
popd

mkdir -p $PREFIX

if [ -z "$MAKEJOBS" ]; then
    MAKEJOBS=$(nproc)
fi

# Build GCC and binutils for the x86_64 target
# ---------------------------------------------------------------------------- #

TARGET64=x86_64-elf

mkdir -p "$DIR/build-x86_64/binutils"
mkdir -p "$DIR/build-x86_64/gcc"

pushd "$DIR/build-x86_64/"
    unset PKG_CONFIG_LIBDIR # Just in case

    pushd binutils
        "$DIR/tarballs/$BINUTILS_DIRECTORY/configure" \
            --target=$TARGET64 \
            --prefix=$PREFIX \
            --with-sysroot \
            --disable-werror || exit 1

        make -j $MAKEJOBS || exit 1
        make install || exit 1
    popd

    pushd gcc
        "$DIR/tarballs/$GCC_DIRECTORY/configure" \
            --target=$TARGET64 \
            --prefix=$PREFIX \
            --disable-nls \
            --with-newlib \
            --with-sysroot \
            --enable-languages=c|| exit 1

        make -j $MAKEJOBS all-gcc all-target-libgcc || exit 1
        make install-gcc install-target-libgcc || exit 1
    popd
popd

touch $PREFIX/build-ok
