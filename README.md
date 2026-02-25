# Multithreaded Performance Analysis using Linux perf

This project explores multithreaded systems programming in a Linux environment using C, POSIX threads (`pthread`), and the Linux `perf` tool. The main goal was to understand how different types of workloads behave when multiple threads run concurrently, and how low-level system metrics help uncover performance bottlenecks.

## Workload Variants

I built four separate multithreaded programs, each designed to stress the system in a different way:

- **CPU-bound:** Heavy computations to fully utilize processor cores.
- **Memory-bound:** Repeated access to large data structures to study memory bandwidth and cache performance.
- **I/O-bound:** Frequent file operations to observe I/O wait time and system overhead.
- **Mixed workload:** A combination of computation, memory access, and I/O to resemble realistic application behavior.

## Scalability Analysis

Each program supports configurable thread counts, allowing scalability testing across different concurrency levels (from a few threads up to 100). Execution time was measured at each level to analyze contention, synchronization overhead, and hardware limitations.

## Performance Profiling with perf

The `perf` tool was used extensively to collect detailed system metrics, including:

- CPU cycles and instructions
- Cache references and cache misses
- Branch mispredictions
- Context switches and CPU migrations
- Memory access behavior

These measurements helped identify inefficiencies related to caching, scheduling, and thread coordination.

## Producer–Consumer Pipeline

To further explore synchronization patterns, a producer–consumer pipeline was implemented using multiple thread pairs. Each pair operates on its own shared buffer and uses mutexes and condition variables to ensure safe coordination without race conditions or busy-waiting. Throughput and execution time were analyzed while varying the number of thread pairs.

## Key Takeaways

- Hands-on experience with Linux systems programming  
- Practical understanding of thread synchronization  
- Scalability testing under high concurrency  
- Hardware-level performance analysis using kernel-supported counters  
