import os
import re
import matplotlib.pyplot as plt

# Automatically detect all perf result files in the current directory
result_files = [f for f in os.listdir(".") if f.endswith(".txt") and f.startswith("perf_results_threads")]

# Data structure to store parsed results
results = []

# Parse each file
for file in result_files:
    file_match = re.search(r'perf_results_threads(\d+)\.txt', file)
    if file_match:
        threads = int(file_match.group(1))
    else:
        continue

    with open(file, 'r') as f:
        content = f.read()

        # Extract performance metrics
        context_switches_match = re.search(r'(\d[\d,]*)\s+context-switches', content)
        cache_misses_match = re.search(r'(\d[\d,]*)\s+cache-misses', content)
        l1_loads_match = re.search(r'(\d[\d,]*)\s+L1-dcache-loads', content)
        l1_load_misses_match = re.search(r'(\d[\d,]*)\s+L1-dcache-load-misses', content)
        llc_loads_match = re.search(r'(\d[\d,]*)\s+LLC-loads', content)
        llc_stores_match = re.search(r'(\d[\d,]*)\s+LLC-stores', content)
        execution_time_match = re.search(r'(\d+\.\d+)\s+seconds time elapsed', content)
        throughput_match = re.search(r'Throughput:\s+([\d,]+\.\d+)\s+items per second', content)

        if context_switches_match and cache_misses_match:
            context_switches = int(context_switches_match.group(1).replace(',', ''))
            cache_misses = int(cache_misses_match.group(1).replace(',', ''))
            l1_loads = int(l1_loads_match.group(1).replace(',', '')) if l1_loads_match else 0
            l1_load_misses = int(l1_load_misses_match.group(1).replace(',', '')) if l1_load_misses_match else 0
            llc_loads = int(llc_loads_match.group(1).replace(',', '')) if llc_loads_match else 0
            llc_stores = int(llc_stores_match.group(1).replace(',', '')) if llc_stores_match else 0
            execution_time = float(execution_time_match.group(1)) if execution_time_match else 0
            throughput = float(throughput_match.group(1).replace(',', '')) if throughput_match else 0

            results.append((threads, context_switches, cache_misses, l1_loads, l1_load_misses, llc_loads, llc_stores, execution_time, throughput))

# Proceed only if we have results
if results:
    results.sort()
    threads = [r[0] for r in results]
    context_switches = [r[1] for r in results]
    cache_misses = [r[2] for r in results]
    l1_loads = [r[3] for r in results]
    l1_load_misses = [r[4] for r in results]
    llc_loads = [r[5] for r in results]
    llc_stores = [r[6] for r in results]
    execution_time = [r[7] for r in results]
    throughput = [r[8] for r in results]

    # Create a figure with individual subplots for each metric
    fig, axes = plt.subplots(4, 2, figsize=(12, 12))
    fig.suptitle('Performance Metrics vs Thread Count')
    
    metrics = [
        (context_switches, 'Context Switches', 'b'),
        (cache_misses, 'Cache Misses', 'r'),
        (l1_loads, 'L1-dcache Loads', 'g'),
        (l1_load_misses, 'L1-dcache Load Misses', 'c'),
        (llc_loads, 'LLC Loads', 'm'),
        (llc_stores, 'LLC Stores', 'orange'),
        (execution_time, 'Execution Time', 'purple'),
        (throughput, 'Throughput', 'brown')
    ]
    
    for ax, (metric_values, title, color) in zip(axes.flatten(), metrics):
        ax.plot(threads, metric_values, marker='o', linestyle='-', color=color, label=title)
        ax.set_xlabel('Thread Count')
        ax.set_ylabel(title)
        ax.set_title(f'{title} vs Thread Count')
        ax.grid(True)
        ax.legend()
    
    plt.tight_layout(rect=[0, 0, 1, 0.96])
    plt.show()
