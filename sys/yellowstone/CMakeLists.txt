add_executable(yellowstone
  hw/pcie.cpp
  yellowstone.cpp
  )
 target_include_directories(yellowstone
   PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
   "${CMAKE_CURRENT_SOURCE_DIR}/../denali/include/")

target_link_libraries(yellowstone
  cxx
  mammoth_lib
  )

set_target_properties(yellowstone PROPERTIES
  COMPILE_FLAGS "${CMAKE_CXX_FLAGS} ${BASE_COMPILE_FLAGS}"
  LINK_FLAGS "${CMAKE_EXE_LINK_FLAGS} ${BASE_LINK_FLAGS}"
  )
