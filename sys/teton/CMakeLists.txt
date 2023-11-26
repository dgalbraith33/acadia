add_executable(teton
  framebuffer/console.cpp
  framebuffer/framebuffer.cpp
  framebuffer/psf.cpp
  keyboard_listener.cpp
  teton.cpp
  )

target_include_directories(teton
   PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
   "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_link_libraries(teton
  glacier
  mammoth
  victoriafalls_yunq
  voyageurs_yunq
  yellowstone_yunq
  )

set_target_properties(teton PROPERTIES
  COMPILE_FLAGS "${CMAKE_CXX_FLAGS} ${BASE_COMPILE_FLAGS}"
  LINK_FLAGS "${CMAKE_EXE_LINK_FLAGS} ${BASE_LINK_FLAGS}"
  )

install(TARGETS teton)
