find_package(Boost REQUIRED)

add_executable(test_asio test_free_functions.cpp test_streambuf.cpp)
target_link_libraries(test_asio PRIVATE util)
add_test_suite(test_asio)