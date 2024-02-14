add_executable(voyageurs
  keyboard/keyboard_driver.cpp
  xhci/trb.cpp
  xhci/trb_ring.cpp
  xhci/xhci_driver.cpp
  voyageurs_server.cpp
  voyageurs.cpp)

target_include_directories(voyageurs
   PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(voyageurs
  glacier
  mammoth
  voyageurs_yunq
  yellowstone_yunq
  )

set_target_properties(voyageurs PROPERTIES
  COMPILE_FLAGS "${CMAKE_CXX_FLAGS} ${BASE_COMPILE_FLAGS}"
  LINK_FLAGS "${CMAKE_EXE_LINK_FLAGS} ${BASE_LINK_FLAGS}"
  )

install(TARGETS voyageurs)

yunq_gen(lib/voyageurs lib voyageurs)
