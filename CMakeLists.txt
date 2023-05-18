cmake_minimum_required(VERSION 3.2)

# Set because our cross compiler can't do dynamic linking?
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")

project(AcadiaOS VERSION 0.0.1 LANGUAGES CXX ASM-ATT)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS True)

add_subdirectory(zion)

add_custom_command(
  OUTPUT disk.img
  COMMAND sudo sh ../scripts/build_image.sh disk.img
  DEPENDS zion
  USES_TERMINAL
)

add_custom_target(qemu
  COMMAND qemu-system-x86_64 -d guest_errors -m 1G -serial stdio -hda disk.img --boot c
  DEPENDS disk.img
  USES_TERMINAL)
