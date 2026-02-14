# Plan

- [x] Build simple TCP server with line-oriented protocol (verify using simple client)
  - Request `hello\n` -> response `world\n`
  - Start from [blocking_tcp_server.cpp](https://www.boost.org/doc/libs/latest/doc/html/boost_asio/example/cpp11/echo/blocking_tcp_echo_server.cpp) and [blocking_tcp_client.cpp](https://www.boost.org/doc/libs/latest/doc/html/boost_asio/example/cpp11/echo/blocking_tcp_echo_client.cpp): both examples are from cpp14 examples; cpp17 and cpp20 examples are all about coroutines

- [x] Check reference implementation with Nginx
  - Spawn a static file server using Nginx using `./run.sh` in Nginx folder
  - Verify and record headers with `curl -v -o /dev/null http://127.0.0.1:8080/files/README.md`
  - Saved record is in `result/curl-output.txt`

- [x] Change line-oriented protocol to HTTP, serve file by having response header and contents on RAM
  - Change your server and client to use \r\n\r\n as delimeter
  - change "hello\n" -> "world\n" to GET request -> 200 OK response which you stored in previous step
  - You need to have file contents on RAM: see below

    ```
    template <typename Executor> struct ReadmeContent {

        ReadmeContent(Executor &executor) {
            auto file = net::stream_file(executor, "README.md",
                                         net::stream_file::flags::read_only);
            buffer.resize(file.size());

            net::read(file, net::buffer(buffer));
        }

        const std::vector<char> &get() { return buffer; }

        std::vector<char> buffer;
    };
    ```
  - Test your server using curl

- [x] Implement a static file server using boost asio, but using synchronous I/O
  - Test sync file operation using boost::asio on `test_streamfile.cpp`
  - Implement `server.cpp` which serves `README.md` file
  - Without having real parsing on request, just serve already read file content with proper response header

- [x] Test the static file server using k6 (load testing tool)
  - Run Nginx using port 8080, run server using port 8081
  - Run k6 according to the instruction in `k6/README.md`
  - Results are stored as `result/k6-on-nginx.txt` and `result/k6-on-our-server.txt`


- [x] Write it async using callbacks
  - Compare these two implementations `example/asio/blocking_tcp_echo_server.cpp` and `example/asio/callback_async_echo_server.cpp`
  - Implement `callback_server.cpp` like given async example
  - Run `io_context` on multiple threads
  - Make file io also async
  - Compare performance using k6

- [x] Write it async using coroutines
  - Compare these two implementations `example/asio/callback_async_echo_server.cpp` and `example/asio/awaitable_async_echo_server.cpp` 
  - Compare these two implementations `example/asio/blocking_tcp_echo_server.cpp` and `example/asio/awaitable_async_echo_server.cpp`
  - Write `callback_server.cpp` and `awaitable_server.cpp` like given asio example
  - Compare performance using k6


---

- [ ] Implement caching

- [ ] Implement keep-alive

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