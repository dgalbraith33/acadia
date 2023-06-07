#! /bin/bash
cmake -B builddbg/ -G Ninja -D CMAKE_CXX_COMPILER=x86_64-elf-gcc -D CMAKE_ASM-ATT_COMPILER=x86_64-elf-gcc -D CMAKE_AR=`which x86_64-elf-ar` -D CMAKE_BUILD_TYPE=Debug 
