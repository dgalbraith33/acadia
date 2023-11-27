
add_executable(testbed
  test.cpp
  )

target_include_directories(testbed
   PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(testbed
  mammoth
)

set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "")

set_target_properties(testbed PROPERTIES
  COMPILE_FLAGS "${CMAKE_CXX_FLAGS} ${BASE_COMPILE_FLAGS}"
  LINK_FLAGS "${CMAKE_EXE_LINK_FLAGS} ${BASE_LINK_FLAGS}"
)

install(TARGETS testbed DESTINATION usr/bin)

