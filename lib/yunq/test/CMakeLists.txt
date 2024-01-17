add_executable(yunq_test yunq_test.cpp)

add_dependencies(yunq_test
  example_yunq)

target_link_libraries(yunq_test 
  Catch2::Catch2WithMain
  example_yunq)

target_include_directories(yunq_test PRIVATE "." "../../../zion/include")

add_test(NAME yunq_test COMMAND $<TARGET_FILE:yunq_test>)

add_dependencies(build_test
    yunq_test)


# Build the yunq manualy rather than using the generator
# because we don't want to link against mammoth and overrite new.
set(target example_yunq)
add_library(example_yunq
  ${CMAKE_CURRENT_SOURCE_DIR}/example/example.yunq.cpp
  )

target_include_directories(example_yunq
  PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/example"
  )

target_link_libraries(${target}
  glacier
  yunq
  zion_stub
)

set(PYTHON "${CMAKE_SOURCE_DIR}/yunq/venv/bin/python")
set(YUNQ "${CMAKE_SOURCE_DIR}/yunq/yunq.py")

add_custom_command(
 OUTPUT example/example.yunq.cpp
 COMMAND ${PYTHON} ${YUNQ} ${CMAKE_CURRENT_SOURCE_DIR}/example/example.yunq
 DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/example/example.yunq
)
