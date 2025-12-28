# Plan

- [ ] Build simple TCP server with line-oriented protocol (verify using simple client)
  - Start from [blocking_tcp_server.cpp](https://www.boost.org/doc/libs/latest/doc/html/boost_asio/example/cpp11/echo/blocking_tcp_echo_server.cpp) and [blocking_tcp_client.cpp](https://www.boost.org/doc/libs/latest/doc/html/boost_asio/example/cpp11/echo/blocking_tcp_echo_client.cpp): both examples are from cpp14 examples; cpp17 and cpp20 examples are all about coroutines
  - Implement line-oriented protocol like hello\n -> world\n
  - change this to GET request -> 200 OK response
  - curl -v https://httpbin.org/get
- [ ] Spawn a static file server using Nginx (verify using curl)
- [ ] Test the static file server using k6 (load testing tool)
- [ ] Implement a static file server using boost asio, but using synchronous I/O
- [ ] Spawn a thread for each connection
- [ ] Write it async using callbacks, run io_context on multiple threads
  - See [async server](https://www.boost.org/doc/libs/latest/doc/html/boost_asio/example/cpp11/echo/async_tcp_echo_server.cpp)
- [ ] Write it async using coroutines
  - See [coroutine server](https://www.boost.org/doc/libs/latest/doc/html/boost_asio/example/cpp20/coroutines/echo_server.cpp)
- [ ] Compare Nginx and our custom solution on AWS EC2
- [ ] Make socket communication and file read interleaved
<!-- - [ ] Try zero-copy using io_uring -->