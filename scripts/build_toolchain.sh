#!/bin/bash

set -e

DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

echo $DIR

ARCH="x86_64"
TARGET="$ARCH-pc-acadia"
TOOLCHAIN="$DIR/../toolchain"
PREFIX="$TOOLCHAIN/local"
BUILD="$DIR/../builddbg"
SYSROOT="$DIR/../sysroot"

BINUTILS_VERSION="2.40"
BINUTILS_NAME="binutils-$BINUTILS_VERSION"
BINUTILS_PKG="${BINUTILS_NAME}.tar.xz"
BINUTILS_BASE_URL="https://ftp.gnu.org/gnu/binutils" 

GCC_VERSION="13.1.0"
GCC_NAME="gcc-$GCC_VERSION"
GCC_PKG="${GCC_NAME}.tar.xz"
GCC_BASE_URL="https://ftp.gnu.org/gnu/gcc"

mkdir -p "$TOOLCHAIN/srcs"
pushd "$TOOLCHAIN/srcs"
  # TODO: Check md5sum
  if [ ! -e "$BINUTILS_PKG" ]; then
    curl -LO "$BINUTILS_BASE_URL/$BINUTILS_PKG"
  fi

  if [ ! -d "$BINUTILS_NAME" ]; then
    tar -xJf ${BINUTILS_PKG}
    pushd $BINUTILS_NAME
    # Set up a baseline to make it easier to create patches in the future.
    git init
    git add .
    git commit -m "base"
    patch -p1 <$TOOLCHAIN/patches/binutils/00-first.patch
    popd
  fi

  if [ ! -e "$GCC_PKG" ]; then
    curl -LO "$GCC_BASE_URL/$GCC_NAME/$GCC_PKG"
  fi

  if [ ! -d "$GCC_NAME" ]; then
    tar -xJf $GCC_PKG
    pushd $GCC_NAME
    # Set up a baseline to make it easier to create patches in the future.
    git init
    git add .
    git commit -m "base"
    patch -p1 <$TOOLCHAIN/patches/gcc/00-first.patch
    patch -p1 <$TOOLCHAIN/patches/gcc/01-libstdc++.patch
    popd
  fi
  

popd

mkdir -p "$TOOLCHAIN/build"
pushd "$TOOLCHAIN/build"
  rm -rf binutils
  mkdir -p binutils
  pushd binutils
      # skip building documentation
      export ac_cv_prog_MAKEINFO=true
      "$TOOLCHAIN"/srcs/$BINUTILS_NAME/configure \
        --prefix=$PREFIX \
        --target="$TARGET" \
        --with-sysroot="$SYSROOT" \
        --enable-shared \
        --disable-nls

      make MAKEINFO=true -j 8
      make install MAKEINFO=true -j 8
  popd

  rm -rf gcc
  mkdir -p gcc
  pushd gcc
    $TOOLCHAIN/srcs/$GCC_NAME/configure \
      --prefix=$PREFIX \
      --target=$TARGET \
      --with-sysroot=$SYSROOT \
      --with-newlib \
      --disable-nls \
      --enable-languages=c,c++


    make -j 8 all-gcc
    make -j 8 all-target-libgcc

    make -j 8 install-gcc install-target-libgcc
  popd

popd
