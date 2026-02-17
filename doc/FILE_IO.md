# File I/O

## sendfile

We cand send file content using `sendfile` system call, which allows us to send file content without copying data between kernel and user space. 
This can improve performance by reducing the number of context switches and memory copies.
But if you want to use application-level caching to avoid thundering herd problem, you need to read file content into user space. Then using `sendfile` may not be suitable for your use case.

## mmap

Memory-mapped file I/O maps a file directly into a process’s address space, allowing access to its contents as if they were in memory. This approach can improve performance by reducing system calls and enabling efficient access patterns. However, it is not suitable for all scenarios—especially when you need to modify file contents or when the file size exceeds available memory. Additionally, memory-mapped I/O does not integrate well with application-level caching, as it offers less control over cache management. With application-level caching, you can implement custom eviction policies and manage memory usage more efficiently, whereas `mmap` relies on the operating system’s memory management policies.