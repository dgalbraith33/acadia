add_executable(yellowstone
  yellowstone.cpp)

target_link_libraries(yellowstone
  cxx
  mammoth_lib
  )

set_target_properties(yellowstone PROPERTIES
  COMPILE_FLAGS "${CMAKE_CXX_FLAGS} ${BASE_COMPILE_FLAGS}"
  LINK_FLAGS "${CMAKE_EXE_LINK_FLAGS} ${BASE_LINK_FLAGS}"
  )
