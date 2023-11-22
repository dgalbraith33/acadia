set(glacier_files
  string/string.cpp
  string/string_builder.cpp
  string/string_view.cpp
  string/str_format.cpp
  string/str_split.cpp
  util/hash.cpp
  )

add_library(glacier STATIC
  ${glacier_files}
  )

target_include_directories(glacier
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
  PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/..")

set_target_properties(glacier PROPERTIES
  COMPILE_FLAGS "${CMAKE_CXX_FLAGS} ${BASE_COMPILE_FLAGS}")

add_library(glacier_kernel STATIC
  ${glacier_files}
  )

target_include_directories(glacier_kernel
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
  PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/..")

set_target_properties(glacier_kernel PROPERTIES
  COMPILE_FLAGS "${CMAKE_CXX_FLAGS} ${BASE_COMPILE_FLAGS} -mcmodel=kernel -mgeneral-regs-only")
