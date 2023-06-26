add_executable(yellowstone
  hw/gpt.cpp
  hw/pcie.cpp
  yellowstone.cpp
  yellowstone_server.cpp
  )
 target_include_directories(yellowstone
   PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(yellowstone
  mammoth
  glacier
  denali_stub
  )

set_target_properties(yellowstone PROPERTIES
  COMPILE_FLAGS "${CMAKE_CXX_FLAGS} ${BASE_COMPILE_FLAGS}"
  LINK_FLAGS "${CMAKE_EXE_LINK_FLAGS} ${BASE_LINK_FLAGS}"
  )


add_library(yellowstonestub
  stub/yellowstone_stub.cpp
  )

target_include_directories(yellowstonestub
   PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
   PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

set_target_properties(yellowstonestub PROPERTIES
  COMPILE_FLAGS "${CMAKE_CXX_FLAGS} ${BASE_COMPILE_FLAGS}"
  LINK_FLAGS "${CMAKE_EXE_LINK_FLAGS} ${BASE_LINK_FLAGS}"
  )
