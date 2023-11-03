add_library(glacier STATIC
  string/string.cpp
  string/string_view.cpp
  string/str_split.cpp
  )

target_include_directories(glacier
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
  PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/..")

set_target_properties(glacier PROPERTIES
  COMPILE_FLAGS "${CMAKE_CXX_FLAGS} ${BASE_COMPILE_FLAGS}")
