cmake_minimum_required(VERSION 3.2)

# Set because our cross compiler can't do dynamic linking?
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")

project(AcadiaOS VERSION 0.0.1 LANGUAGES CXX ASM-ATT)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS True)

set(BASE_COMPILE_FLAGS "-ffreestanding -fno-rtti -fno-exceptions -mincoming-stack-boundary=3")
set(BASE_LINK_FLAGS "-nostdlib")

if (enable_testing)
  include(CTest)
  find_package(Catch2 3 REQUIRED)
  find_program(MEMORYCHECK_COMMAND valgrind)
  set(MEMORYCHECK_COMMAND_OPTIONS "--trace-children=yes --leak-check=full")
  add_custom_target(build_test)
endif()

add_subdirectory(zion)
add_subdirectory(yunq)
add_subdirectory(lib)
add_subdirectory(usr)
add_subdirectory(sys)
