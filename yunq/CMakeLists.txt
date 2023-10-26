macro(yunq_gen dir include_dir name)
  set(file_bundle ${name}_yunq_files)
  set(target ${name}_yunq)
  set(${file_bundle}
    ${CMAKE_CURRENT_SOURCE_DIR}/${dir}/${name}.yunq.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/${dir}/${name}.yunq.client.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/${dir}/${name}.yunq.server.cpp
    )

  add_library(${target}
    ${${file_bundle}}
    )

  target_include_directories(${target}
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/${include_dir}"
    )

  target_link_libraries(${target}
    mammoth
    )
  set_target_properties(${target} PROPERTIES
    COMPILE_FLAGS "${CMAKE_CXX_FLAGS} ${BASE_COMPILE_FLAGS}"
    LINK_FLAGS "${CMAKE_EXE_LINK_FLAGS} ${BASE_LINK_FLAGS}"
    )

  set(PYTHON "${CMAKE_SOURCE_DIR}/yunq/venv/bin/python")
  set(YUNQ "${CMAKE_SOURCE_DIR}/yunq/yunq.py")

  add_custom_command(
    OUTPUT ${${file_bundle}}
    COMMAND ${PYTHON} ${YUNQ} ${CMAKE_CURRENT_SOURCE_DIR}/${dir}/${name}.yunq
    DEPENDS ${dir}/${name}.yunq
    )
endmacro()
