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

set(_COMPILE_FLAGS "${CMAKE_CXX_FLAGS} -c -ffreestanding -fno-rtti -fno-exceptions -nostdlib -mabi=sysv -mgeneral-regs-only")
set(_LINK_FLAGS "-nostdlib")

set_target_properties(mammoth_lib PROPERTIES
  COMPILE_FLAGS "${_COMPILE_FLAGS}"
  LINK_FLAGS "${CMAKE_EXE_LINK_FLAGS} ${_LINK_FLAGS}"
  )
