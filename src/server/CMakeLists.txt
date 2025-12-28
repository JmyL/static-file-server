Find_package(Boost REQUIRED)

add_executable(server server.cpp)
target_include_directories(server PRIVATE ${CMAKE_SOURCE_DIR}/src ${BOOST_INCLUDE_DIRS})
