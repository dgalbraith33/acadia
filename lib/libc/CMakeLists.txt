
add_library(c STATIC
  src/malloc.cpp
  src/stdio.cpp
  )

target_include_directories(c
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(c
  zion_lib
  )

set_target_properties(c PROPERTIES
  COMPILE_FLAGS "${CMAKE_CXX_FLAGS} ${BASE_COMPILE_FLAGS}")
