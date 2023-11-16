cmake_minimum_required(VERSION 3.2)

# Set because our cross compiler can't do dynamic linking?
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")

project(AcadiaOS VERSION 0.0.1 LANGUAGES CXX ASM-ATT)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS True)

set(BASE_COMPILE_FLAGS "-ffreestanding -fno-rtti -fno-exceptions -mgeneral-regs-only")
set(BASE_LINK_FLAGS "-nostdlib")

add_subdirectory(zion)
add_subdirectory(lib)
add_subdirectory(yunq)
add_subdirectory(sys)
