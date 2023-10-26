add_executable(denali
  ahci/ahci_device.cpp
  ahci/ahci_driver.cpp
  ahci/command.cpp
  denali.cpp
  denali_server.cpp
  )


target_include_directories(denali
   PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(denali
  denali_yunq
  glacier
  mammoth
  yellowstone_yunq
  )

set_target_properties(denali PROPERTIES
  COMPILE_FLAGS "${CMAKE_CXX_FLAGS} ${BASE_COMPILE_FLAGS}"
  LINK_FLAGS "${CMAKE_EXE_LINK_FLAGS} ${BASE_LINK_FLAGS}"
  )

yunq_gen(lib/denali lib denali)
