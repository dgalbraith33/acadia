add_library(mammoth STATIC
  ipc/channel.cpp
  ipc/endpoint_client.cpp
  ipc/endpoint_server.cpp
  ipc/port_client.cpp
  ipc/port_server.cpp
  src/debug.cpp
  src/init.cpp
  src/memory_region.cpp
  src/new.cpp
  src/process.cpp
  src/thread.cpp
  sync/mutex.cpp
  sync/semaphore.cpp
  )

target_include_directories(mammoth
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
  "${CMAKE_CURRENT_SOURCE_DIR}/.."
  )

target_link_libraries(mammoth
  glacier
  c
  zion_stub)

set_target_properties(mammoth PROPERTIES
  COMPILE_FLAGS "${CMAKE_CXX_FLAGS} ${BASE_COMPILE_FLAGS}"
  LINK_FLAGS "${CMAKE_EXE_LINK_FLAGS} ${BASE_LINK_FLAGS}"
  )
