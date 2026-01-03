# Plan

- [ ] Build simple TCP server with line-oriented protocol (verify using simple client)
  - Start from [blocking_tcp_server.cpp](https://www.boost.org/doc/libs/latest/doc/html/boost_asio/example/cpp11/echo/blocking_tcp_echo_server.cpp) and [blocking_tcp_client.cpp](https://www.boost.org/doc/libs/latest/doc/html/boost_asio/example/cpp11/echo/blocking_tcp_echo_client.cpp): both examples are from cpp14 examples; cpp17 and cpp20 examples are all about coroutines
  - Implement line-oriented protocol like hello\n -> world\n
- [ ] Spawn a static file server using Nginx (verify using curl)
  - curl -v -o /dev/null http://127.0.0.1:8080/files/README.md
    ```{bash}
> GET /files/README.md HTTP/1.1
> Host: 127.0.0.1:8080
> User-Agent: curl/8.15.0
> Accept: */*
> 
* Request completely sent off
< HTTP/1.1 200 OK
< Server: nginx/1.29.4
< Date: Sat, 03 Jan 2026 15:22:53 GMT
< Content-Type: text/plain
< Content-Length: 1192
< Last-Modified: Sun, 28 Dec 2025 17:25:28 GMT
< Connection: keep-alive
< ETag: "69516808-4a8"
< Accept-Ranges: bytes
< 
```

- [ ] Change line-oriented protocol to HTTP
  - change "hello\n" -> "world\n" to GET request -> 200 OK response
  - curl -v https://httpbin.org/get
- [ ] Test the static file server using k6 (load testing tool)
- [ ] Implement a static file server using boost asio, but using synchronous I/O
- [ ] Spawn a thread for each connection
- [ ] Write it async using callbacks, run io_context on multiple threads
  - See [async server](https://www.boost.org/doc/libs/latest/doc/html/boost_asio/example/cpp11/echo/async_tcp_echo_server.cpp)
- [ ] Write it async using coroutines
  - See [coroutine server](https://www.boost.org/doc/libs/latest/doc/html/boost_asio/example/cpp20/coroutines/echo_server.cpp)
- [ ] Compare Nginx and our custom solution on AWS EC2
- [ ] Make socket communication and file read interleaved
- [ ] Use memory pool
<!-- - [ ] Try zero-copy using io_uring -->