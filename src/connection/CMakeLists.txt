add_library(client INTERFACE)
target_include_directories(client INTERFACE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(client INTERFACE util)