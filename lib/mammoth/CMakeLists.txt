add_library(mammoth STATIC
  file/file.cpp
  input/keyboard.cpp
  ipc/channel.cpp
  ipc/endpoint_client.cpp
  ipc/endpoint_server.cpp
  ipc/port_client.cpp
  ipc/port_server.cpp
  proc/process.cpp
  proc/thread.cpp
  sync/mutex.cpp
  sync/semaphore.cpp
  util/debug.cpp
  util/init.cpp
  util/memory_region.cpp
  util/new.cpp
  )

target_include_directories(mammoth
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
  "${CMAKE_CURRENT_SOURCE_DIR}/.."
  )

target_link_libraries(mammoth
  glacier
  victoriafalls_yunq
  yellowstone_yunq
  voyageurs_yunq
  zion_stub
  )

set_target_properties(mammoth PROPERTIES
  COMPILE_FLAGS "${CMAKE_CXX_FLAGS} ${BASE_COMPILE_FLAGS}"
  LINK_FLAGS "${CMAKE_EXE_LINK_FLAGS} ${BASE_LINK_FLAGS}"
  )
