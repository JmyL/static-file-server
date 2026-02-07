add_executable(server server.cpp)
target_compile_definitions(server PRIVATE -DBOOST_ASIO_HAS_IO_URING=1)
target_link_libraries(server PRIVATE connection uring)