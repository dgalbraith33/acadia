add_executable(victoriafalls
  fs/ext2/ext2_block_reader.cpp
  fs/ext2/ext2_driver.cpp
  fs/ext2/inode_table.cpp
  victoriafalls.cpp
  victoriafalls_server.cpp
  )

target_include_directories(victoriafalls
   PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
   "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_link_libraries(victoriafalls
  denali_yunq
  glacier
  mammoth
  victoriafalls_yunq
  yellowstone_yunq
  )

set_target_properties(victoriafalls PROPERTIES
  COMPILE_FLAGS "${CMAKE_CXX_FLAGS} ${BASE_COMPILE_FLAGS}"
  LINK_FLAGS "${CMAKE_EXE_LINK_FLAGS} ${BASE_LINK_FLAGS}"
  )

yunq_gen(lib/victoriafalls lib victoriafalls)
