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
  cxx
  glacier
  mammoth_lib
  )

set_target_properties(denali PROPERTIES
  COMPILE_FLAGS "${CMAKE_CXX_FLAGS} ${BASE_COMPILE_FLAGS}"
  LINK_FLAGS "${CMAKE_EXE_LINK_FLAGS} ${BASE_LINK_FLAGS}"
  )

add_library(libdenali
  client/denali_client.cpp
  )

target_include_directories(libdenali
   PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
   PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

 target_link_libraries(libdenali
   mammoth_lib)

set_target_properties(libdenali PROPERTIES
  COMPILE_FLAGS "${CMAKE_CXX_FLAGS} ${BASE_COMPILE_FLAGS}"
  LINK_FLAGS "${CMAKE_EXE_LINK_FLAGS} ${BASE_LINK_FLAGS}"
  )
