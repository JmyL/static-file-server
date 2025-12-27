# Plan

- [ ] Build simple TCP server with line-oriented protocol (verify using simple client)
- [ ] Spawn a static file server using Nginx (verify using curl)
- [ ] Test the static file server using k6 (load testing tool)
- [ ] Implement a static file server using boost asio, but using synchronous I/O
- [ ] Spawn a thread for each connection
- [ ] Write it async using callbacks, run io_context on multiple threads
- [ ] Write it async using coroutines
- [ ] Compare Nginx and our custom solution on AWS EC2
- [ ] Make socket communication and file read interleaved
<!-- - [ ] Try zero-copy using io_uring -->
