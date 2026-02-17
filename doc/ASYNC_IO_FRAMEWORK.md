<!-- photon, PhotonLibOS -->
ASIO, CPU affinity를 높이기 위해 CPU 마다 별도 io_context를 만들어서 사용하는 걸 검토해 보자. 그러면 per-cpu cache도 활용할 수 있다.
대신 session은 round-robin으로 io_context에 할당하거나, 혹은 request 마다 hashing해서 io_context에 할당하면 cache hit rate을 높일 수 있다. 다만 단일 CPU에 부하가 집중될 수 있겠네... 이건 무조건 피해야 하는데, 그럼 이 방법은 못 쓰는 건가?

Seastar는? Shared-nothing? 이건 단일 파일 요청이 계속 들어오면 여러 CPU에 잘 나누어 주나? 짜기 나름인가?

https://www.reddit.com/r/cpp/comments/1fg1kom/seastar_vs_boost_asio/

SPDK: storage.
DPDK: network.

Rust Tokio/Glommio

- [ ] Learn "work-stealing"
- [ ] Seastar