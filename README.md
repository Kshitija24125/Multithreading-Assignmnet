**Multithreaded Performance Analysis using Linux perf**


This project explores multithreaded systems programming in a Linux environment using C, POSIX threads (`pthread`), and the Linux `perf` tool. The main goal was to understand how different types of workloads behave when multiple threads run concurrently, and how low-level system metrics help uncover performance bottlenecks.

I built four separate multithreaded programs, each designed to stress the system in a different way. The CPU-bound program focuses on heavy computations to fully utilize processor cores. The memory-bound version repeatedly accesses large data structures to study memory bandwidth and cache performance. The I/O-bound program performs frequent file operations to observe I/O wait time and system overhead. A mixed workload combines computation, memory access, and I/O to resemble more realistic application behavior.

Each program allows configurable thread counts, making it possible to test scalability across different levels of concurrency, from a few threads up to 100. By measuring execution time and profiling performance at each level, I analyzed how contention, synchronization overhead, and hardware limits affect overall performance.

The `perf` tool was used extensively to collect detailed metrics such as CPU cycles, instructions, cache references, cache misses, branch mispredictions, context switches, and CPU migrations. These insights helped identify inefficiencies related to caching, scheduling, and thread coordination.

To further study synchronization, I implemented a producer–consumer pipeline with multiple thread pairs. Each pair operates on its own shared buffer and uses mutexes and condition variables to ensure safe coordination without race conditions or busy-waiting. Throughput and execution time were analyzed while varying the number of thread pairs.

Overall, this project strengthened my understanding of Linux systems programming, thread synchronization, scalability behavior, and hardware-level performance profiling.
