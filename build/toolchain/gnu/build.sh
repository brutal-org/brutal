#!/bin/bash
set -e

# ----- Configs -------------------------------------------------------------- #

if [ $# == 0 ]
then
    echo "use build.sh <target>"
    echo "target may be: "
    echo "  - x86_64"
    echo "  - riscv64"
    exit -1
fi

TARGET=$1-elf

BINUTILS_VERSION=2.36
BINUTILS_DIRECTORY="binutils-$BINUTILS_VERSION"
BINUTILS_FILENAME="$BINUTILS_DIRECTORY.tar.gz"
BINUTILS_URL="http://ftp.gnu.org/gnu/binutils/$BINUTILS_FILENAME"

GCC_VERSION=11.1.0
GCC_DIRECTORY="gcc-$GCC_VERSION"
GCC_FILENAME="gcc-$GCC_VERSION.tar.gz"
GCC_URL="http://ftp.gnu.org/gnu/gcc/$GCC_DIRECTORY/$GCC_FILENAME"

# ---------------------------------------------------------------------------- #

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

PREFIX="$DIR/local"

if [ -e "$PREFIX/$TARGET/build-ok" ]; then
    echo "The toolchain is built and ready :)"
    exit 0
fi

cd "$DIR"

mkdir -p tarballs

source "$DIR/use.sh"

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

        # FIXME: What if this fail ?
        echo "Download gcc prerequisites..."

        pushd $GCC_DIRECTORY
            ./contrib/download_prerequisites
        popd
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
mkdir -p "$DIR/build/$TARGET/binutils"
mkdir -p "$DIR/build/$TARGET/gcc"

pushd "$DIR/build/$TARGET"
    unset PKG_CONFIG_LIBDIR # Just in case

    pushd binutils
        "$DIR/tarballs/$BINUTILS_DIRECTORY/configure" \
            --target=$TARGET \
            --prefix=$PREFIX \
            --with-sysroot \
            --disable-werror

        make -j $MAKEJOBS
        make install
    popd

    pushd gcc
        "$DIR/tarballs/$GCC_DIRECTORY/configure" \
            --target=$TARGET \
            --prefix=$PREFIX \
            --disable-nls \
            --with-newlib \
            --with-sysroot \
            --enable-languages=c

        make -j $MAKEJOBS all-gcc all-target-libgcc
        make install-gcc install-target-libgcc
    popd
popd

touch $PREFIX/$TARGET/build-ok
