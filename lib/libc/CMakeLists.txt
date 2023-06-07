
add_library(libc STATIC
  src/malloc.cpp
  )

target_include_directories(libc
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(libc
  zion_lib
  )

set_target_properties(libc PROPERTIES
  COMPILE_FLAGS "${CMAKE_CXX_FLAGS} -ffreestanding -mgeneral-regs-only")
