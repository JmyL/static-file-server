find_package(Boost REQUIRED)

add_executable(test_asio test_asio.cpp)
add_test_suite(test_asio)
