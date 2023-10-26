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

add_library(denali_yunq
  lib/denali/denali.yunq.client.cpp
  lib/denali/denali.yunq.server.cpp
  lib/denali/denali.yunq.cpp
  )

target_include_directories(denali_yunq
   PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
   PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/lib")

 target_link_libraries(denali_yunq
   mammoth)

set_target_properties(denali_yunq PROPERTIES
  COMPILE_FLAGS "${CMAKE_CXX_FLAGS} ${BASE_COMPILE_FLAGS}"
  LINK_FLAGS "${CMAKE_EXE_LINK_FLAGS} ${BASE_LINK_FLAGS}"
  )
