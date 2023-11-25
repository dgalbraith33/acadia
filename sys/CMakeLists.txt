set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "")

add_subdirectory(denali)
add_subdirectory(teton)
add_subdirectory(victoriafalls)
add_subdirectory(voyageurs)
add_subdirectory(yellowstone)

set(SYS_BUNDLE
  denali
  victoriafalls
  yellowstone)

