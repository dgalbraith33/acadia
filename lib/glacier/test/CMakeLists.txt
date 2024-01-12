find_package(Catch2 3 REQUIRED)

add_subdirectory(container)

add_custom_target(build_test)
add_dependencies(build_test
  glc_vec_test)

