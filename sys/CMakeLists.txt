
set(_COMPILE_FLAGS "-ffreestanding -mgeneral-regs-only")
set(_LINK_FLAGS "-nostdlib")
set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "")

add_executable(test
  test.cpp
  )

target_link_libraries(test
  mammoth_lib)

set_target_properties(test
  PROPERTIES
  COMPILE_FLAGS "${CMAKE_CXX_FLAGS} ${_COMPILE_FLAGS}"
  LINK_FLAGS "${CMAKE_EXE_LINK_FLAGS} ${_LINK_FLAGS}"
  )

add_executable(test2
  test2.cpp)

target_link_libraries(test2
  mammoth_lib)

set_target_properties(test2
  PROPERTIES
  COMPILE_FLAGS "${CMAKE_CXX_FLAGS} ${_COMPILE_FLAGS}"
  LINK_FLAGS "${CMAKE_EXE_LINK_FLAGS} ${_LINK_FLAGS}"
  )
