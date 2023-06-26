add_executable(denali
  ahci/ahci_device.cpp
  ahci/ahci_driver.cpp
  ahci/command.cpp
  denali.cpp
  denali_server.cpp
  )


target_include_directories(denali
   PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
   "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_link_libraries(denali
  glacier
  mammoth
  yellowstone_stub
  )

set_target_properties(denali PROPERTIES
  COMPILE_FLAGS "${CMAKE_CXX_FLAGS} ${BASE_COMPILE_FLAGS}"
  LINK_FLAGS "${CMAKE_EXE_LINK_FLAGS} ${BASE_LINK_FLAGS}"
  )

add_library(denali_stub
  client/denali_client.cpp
  )

target_include_directories(denali_stub
   PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
   PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

 target_link_libraries(denali_stub
   mammoth)

set_target_properties(denali_stub PROPERTIES
  COMPILE_FLAGS "${CMAKE_CXX_FLAGS} ${BASE_COMPILE_FLAGS}"
  LINK_FLAGS "${CMAKE_EXE_LINK_FLAGS} ${BASE_LINK_FLAGS}"
  )
