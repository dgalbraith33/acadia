set(yunq_files
  message_view.cpp
  serialize.cpp
  )

add_library(yunq STATIC
  ${yunq_files}
  )

target_link_libraries(yunq
  glacier)

target_include_directories(yunq
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
  PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/..")

set_target_properties(yunq PROPERTIES
  COMPILE_FLAGS "${CMAKE_CXX_FLAGS} ${BASE_COMPILE_FLAGS}")
