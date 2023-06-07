add_library(cxx STATIC
  src/new.cpp
  )

target_include_directories(cxx
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(cxx
  c
  zion_lib
  )

set_target_properties(cxx PROPERTIES
  COMPILE_FLAGS "${CMAKE_CXX_FLAGS} ${BASE_COMPILE_FLAGS}")
