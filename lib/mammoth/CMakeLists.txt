add_library(mammoth_lib STATIC
  src/channel.cpp
  src/debug.cpp
  src/process.cpp
  src/thread.cpp
  )

target_include_directories(mammoth_lib
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(mammoth_lib
  zion_lib)

set_target_properties(mammoth_lib PROPERTIES
  COMPILE_FLAGS "${CMAKE_CXX_FLAGS} ${BASE_COMPILE_FLAGS}"
  LINK_FLAGS "${CMAKE_EXE_LINK_FLAGS} ${BASE_LINK_FLAGS}"
  )
