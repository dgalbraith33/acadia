cmake_minimum_required(VERSION 3.2)

# Set because our cross compiler can't do dynamic linking?
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")

project(AcadiaOS VERSION 0.0.1 LANGUAGES CXX ASM-ATT)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS True)

set(BASE_COMPILE_FLAGS "-ffreestanding -fno-rtti -fno-exceptions -mgeneral-regs-only")
set(BASE_LINK_FLAGS "-nostdlib")

set(PYTHON "${CMAKE_SOURCE_DIR}/yunq/venv/bin/python")
set(YUNQ "${CMAKE_SOURCE_DIR}/yunq/yunq.py")

macro(yunq_gen dir include_dir name)
  set(file_bundle ${name}_yunq_files)
  set(target ${name}_yunq)
  set(${file_bundle}
    ${CMAKE_CURRENT_SOURCE_DIR}/${dir}/${name}.yunq.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/${dir}/${name}.yunq.client.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/${dir}/${name}.yunq.server.cpp
    )

  add_library(${target}
    ${${file_bundle}}
    )

  target_include_directories(${target}
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/${include_dir}"
    )

  target_link_libraries(${target}
    mammoth
    )
  set_target_properties(${target} PROPERTIES
    COMPILE_FLAGS "${CMAKE_CXX_FLAGS} ${BASE_COMPILE_FLAGS}"
    LINK_FLAGS "${CMAKE_EXE_LINK_FLAGS} ${BASE_LINK_FLAGS}"
    )

  add_custom_command(
    OUTPUT ${${file_bundle}}
    COMMAND ${PYTHON} ${YUNQ} ${CMAKE_CURRENT_SOURCE_DIR}/${dir}/${name}.yunq
    DEPENDS ${dir}/${name}.yunq
    )
endmacro()

add_subdirectory(zion)
add_subdirectory(lib)
add_subdirectory(sys)

# Use machine q35 to access PCI devices.
set(QEMU_CMD qemu-system-x86_64 -machine q35 -d guest_errors -m 1G -serial stdio -hda disk.img)

add_custom_command(
  OUTPUT disk.img
  COMMAND sudo sh ../scripts/build_image.sh disk.img
  DEPENDS zion yellowstone denali victoriafalls
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
