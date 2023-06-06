cmake_minimum_required(VERSION 3.2)

# Set because our cross compiler can't do dynamic linking?
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")

project(AcadiaOS VERSION 0.0.1 LANGUAGES CXX ASM-ATT)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS True)

add_subdirectory(zion)
add_subdirectory(lib)
add_subdirectory(sys)

set(QEMU_CMD qemu-system-x86_64 -d guest_errors -m 1G -serial stdio -hda disk.img)

add_custom_command(
  OUTPUT disk.img
  COMMAND sudo sh ../scripts/build_image.sh disk.img
  DEPENDS zion test test2
  USES_TERMINAL
)

add_custom_target(qemu
  COMMAND ${QEMU_CMD}
  DEPENDS disk.img
  USES_TERMINAL)

add_custom_target(qemu-dbg
  COMMAND ${QEMU_CMD} -S -s
  DEPENDS disk.img
  USES_TERMINAL)

add_custom_target(qemu-int
  COMMAND ${QEMU_CMD} -d int
  DEPENDS disk.img
  USES_TERMINAL)
