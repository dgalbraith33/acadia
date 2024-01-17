add_executable(glc_vec_test vector.cpp)
target_link_libraries(glc_vec_test glacier Catch2::Catch2WithMain)
target_include_directories(glc_vec_test PRIVATE "../..")
add_test(NAME glc_vec_test COMMAND $<TARGET_FILE:glc_vec_test>)

add_dependencies(build_test
    glc_vec_test)

