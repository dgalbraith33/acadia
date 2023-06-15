#! /bin/bash

CWD="$(pwd)"
BIN=$CWD/toolchain/local/bin
GCC=$BIN/x86_64-pc-acadia-gcc
AR=$BIN/x86_64-pc-acadia-ar
cmake -B builddbg/ -G Ninja -D CMAKE_CXX_COMPILER=${GCC} -D CMAKE_ASM-ATT_COMPILER=${GCC} -D CMAKE_AR=${AR} -D CMAKE_BUILD_TYPE=Debug 
