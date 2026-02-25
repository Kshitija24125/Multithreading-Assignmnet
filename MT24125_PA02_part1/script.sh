#!/bin/bash

# Define the different number of producer-consumer threads for testing
thread_counts=(2 4 6 8 10 15 18 20 22 23 26 27 29 30 35 40 50 62 65 68 70 80 90 92  97 100)  

# Source code file
source_file="pro-con.c"
executable="./p"

# Compile the C program (Check if compilation succeeds)
gcc -o p pro-con.c -pthread
if [ $? -ne 0 ]; then
    echo "Compilation failed! Check your C program for errors."
    exit 1
fi

# Loop through each thread count
for threads in "${thread_counts[@]}"; do
    output_file="perf_results_threads${threads}.txt"
    temp_output_file="temp_output.txt"

    touch "$temp_output_file"


    if [ -f "$executable" ]; then
        $executable "${threads}" > "$temp_output_file"
    else
        echo "Executable not found! Check compilation step."
        exit 1
    fi

    cat "$temp_output_file" > "$output_file"


    sudo perf stat -e cycles,instructions,branches,cache-references,cache-misses,context-switches --append --output=$output_file -- $executable "${threads}" > /dev/null 2>&1
    sudo perf stat -e L1-dcache-loads,L1-dcache-load-misses,LLC-loads,LLC-stores --append --output=$output_file -- $executable "${threads}" > /dev/null 2>&1


    rm "$temp_output_file"
done

