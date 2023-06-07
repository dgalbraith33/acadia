add_library(libcxx STATIC
  src/new.cpp
  )

target_include_directories(libcxx
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(libcxx
  libc
  zion_lib
  )

set_target_properties(libcxx PROPERTIES
  COMPILE_FLAGS "${CMAKE_CXX_FLAGS} -ffreestanding -mgeneral-regs-only")
