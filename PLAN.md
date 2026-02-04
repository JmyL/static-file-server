# Plan

- [x] Build simple TCP server with line-oriented protocol (verify using simple client)
  - Start from [blocking_tcp_server.cpp](https://www.boost.org/doc/libs/latest/doc/html/boost_asio/example/cpp11/echo/blocking_tcp_echo_server.cpp) and [blocking_tcp_client.cpp](https://www.boost.org/doc/libs/latest/doc/html/boost_asio/example/cpp11/echo/blocking_tcp_echo_client.cpp): both examples are from cpp14 examples; cpp17 and cpp20 examples are all about coroutines
  - Implement line-oriented protocol like hello\n -> world\n

- [x] Spawn a static file server using Nginx (verify using curl)
  - curl -v -o /dev/null http://127.0.0.1:8080/files/README.md
  - Store header of request and response

- [x] Change line-oriented protocol to HTTP
  - Change your server and client to use \r\n\r\n as delimeter
  - change "hello\n" -> "world\n" to GET request -> 200 OK response which you stored in previous step
  - You may need to put additional file contents or you can modify Content-Length as 0 for test
  - Test your server using curl

- [x] Test the static file server using k6 (load testing tool)

- [ ] Implement a static file server using boost asio, but using synchronous I/O

- [ ] Write it async using callbacks, run io_context on multiple threads
  - See [async server](https://www.boost.org/doc/libs/latest/doc/html/boost_asio/example/cpp11/echo/async_tcp_echo_server.cpp)
- [ ] Write it async using coroutines
  - See [coroutine server](https://www.boost.org/doc/libs/latest/doc/html/boost_asio/example/cpp20/coroutines/echo_server.cpp)

---

- [ ] Compare Nginx and our custom solution on AWS EC2

- [ ] Make socket communication and file read interleaved

- [ ] Use memory pool

<!-- - [ ] Try zero-copy using io_uring -->

sungsik@fedora:~$ curl -v -o /dev/null http://127.0.0.1:8080/files/README.md
  % Total    % Received % Xferd  Average Speed   Time    Time     Time  Current
                                 Dload  Upload   Total   Spent    Left  Speed
  0     0    0     0    0     0      0      0 --:--:-- --:--:-- --:--:--     0*   Trying 127.0.0.1:8080...
* Connected to 127.0.0.1 (127.0.0.1) port 8080
* using HTTP/1.x
> GET /files/README.md HTTP/1.1
> Host: 127.0.0.1:8080
> User-Agent: curl/8.15.0
> Accept: */*
> 
* Request completely sent off
< HTTP/1.1 200 OK
< Server: nginx/1.29.4
< Date: Wed, 04 Feb 2026 18:39:35 GMT
< Content-Type: text/plain
< Content-Length: 1192
< Last-Modified: Sun, 28 Dec 2025 17:25:28 GMT
< Connection: keep-alive
< ETag: "69516808-4a8"
< Accept-Ranges: bytes
< 
{ [1192 bytes data]
100  1192  100  1192    0     0   268k      0 --:--:-- --:--:-- --:--:--  291k
* Connection #0 to host 127.0.0.1 left intact