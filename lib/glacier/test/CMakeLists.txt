find_package(Catch2 3 REQUIRED)
find_program(MEMORYCHECK_COMMAND valgrind)
set(MEMORYCHECK_COMMAND_OPTIONS "--trace-children=yes --leak-check=full")

add_subdirectory(container)

add_custom_target(build_test)
add_dependencies(build_test
  glc_vec_test)

