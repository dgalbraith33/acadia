add_library(glacier STATIC
  string/string.cpp)

target_include_directories(glacier
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
  PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/..")
