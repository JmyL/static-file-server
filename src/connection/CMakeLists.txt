find_package(Boost REQUIRED)

add_library(connection INTERFACE)
target_include_directories(connection INTERFACE ${CMAKE_SOURCE_DIR}/src ${BOOST_INCLUDE_DIRS})