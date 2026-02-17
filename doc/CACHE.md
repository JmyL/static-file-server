1. heap allocator
    - A memory management technique that allows for dynamic allocation and deallocation of memory at runtime.
    - Memory is allocated from a large pool called the heap, which can grow or shrink as needed.
    - Heap allocators typically use algorithms like first-fit, best-fit, or buddy system to manage free memory and minimize fragmentation.
    - This approach provides flexibility but can lead to fragmentation and performance issues if not managed properly.
    - Heap allocators are commonly used in general-purpose programming languages and applications that require dynamic memory management.
1. slab allocator
    - A memory management technique that divides memory into small, fixed-size blocks called slabs.
    - Each slab is designed to hold objects of a specific type or size, allowing for efficient allocation and deallocation of memory.
    - This approach minimizes fragmentation and improves performance by reducing the overhead associated with dynamic memory allocation.
    - Slab allocators are commonly used in operating systems and high-performance applications to manage memory efficiently.
    - Libraries
        1. **libslab**  
           - C로 작성된 slab allocator 라이브러리  
           - [GitHub: libslab](https://github.com/stephenrkell/libslab)

        2. **jemalloc**  
           - 고성능 메모리 allocator로, 내부적으로 slab 방식 사용  
           - [jemalloc 공식 사이트](https://jemalloc.net/)  
           - [GitHub: jemalloc](https://github.com/jemalloc/jemalloc)

        3. **tcmalloc**  
           - Google의 고성능 메모리 allocator, slab 기반  
           - [GitHub: tcmalloc](https://github.com/google/tcmalloc)

        4. **Boost Pool**  
           - Boost C++ 라이브러리의 pool allocator  
           - slab allocator와 유사한 방식으로 고정 크기 블록 관리  
           - [Boost Pool 공식 문서](https://www.boost.org/doc/libs/release/libs/pool/doc/html/index.html)


Segmented File Cache

> Intrusive free list / embedded free list: A technique where the free blocks of memory are linked together using pointers stored within the blocks themselves, eliminating the need for separate data structures to manage free memory.

Per-CPU cache: https://github.com/google/tcmalloc/blob/master/docs/design.md#per-cpu-cache
: boost asio의 async model과는 잘 맞지 않음.

- Normal page size: 4KB
- [ ] Hugepages: 2MB or 1GB (my case, 2MB), you need to reserve 5~10GB for OS and other processes
- [ ] tcalloc
- [ ] boost pool