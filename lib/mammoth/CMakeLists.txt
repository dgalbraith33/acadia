add_library(mammoth STATIC
  src/channel.cpp
  src/debug.cpp
  src/endpoint_client.cpp
  src/endpoint_server.cpp
  src/init.cpp
  src/memory_region.cpp
  src/new.cpp
  src/process.cpp
  src/port_client.cpp
  src/port_server.cpp
  src/thread.cpp
  )

target_include_directories(mammoth
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(mammoth
  glacier
  c
  zion_stub)

set_target_properties(mammoth PROPERTIES
  COMPILE_FLAGS "${CMAKE_CXX_FLAGS} ${BASE_COMPILE_FLAGS}"
  LINK_FLAGS "${CMAKE_EXE_LINK_FLAGS} ${BASE_LINK_FLAGS}"
  )
