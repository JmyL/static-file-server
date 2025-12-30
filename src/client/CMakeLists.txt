find_package(Boost REQUIRED)

add_executable(client client.cpp)
target_include_directories(client PRIVATE ${CMAKE_SOURCE_DIR}/src ${BOOST_INCLUDE_DIRS})