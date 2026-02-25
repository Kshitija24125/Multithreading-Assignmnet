#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <chrono>
#include <iostream>
#include<ctime>
#include<iomanip>
using namespace std;
using namespace std::chrono;

// Global variables
int* a;                
int a_size;            
int num_threads;       
int max_value = INT_MIN;  
pthread_mutex_t max_mutex;  

struct ThreadData {
    int start;
    int end;
};

// Function to find the maximum in each segment
void* maximum(void* arg)
{
    ThreadData* data = (ThreadData*)arg;
    int start = data->start;
    int end = data->end;

    int local_max = INT_MIN;

    for (int i = start; i < end; i++) {
        if (a[i] > local_max)
            local_max = a[i];
    }

    pthread_mutex_lock(&max_mutex);
    if (local_max > max_value) {
        max_value = local_max;
    }
    pthread_mutex_unlock(&max_mutex);

    return NULL;
}

int main(int argc, char* argv[])
{
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    if (argc < 4) {
        printf("Usage: %s <a_size> <num_threads> <array_elements>\n", argv[0]);
        return 1;
    }

    a_size = atoi(argv[1]);
    num_threads = atoi(argv[2]);

    if (argc != a_size + 3) {
        printf("Error: Provide exactly %d array elements.\n", a_size);
        return 1;
    }

    // if (num_threads <= 0 || num_threads > a_size) {
    //     printf("Error: Number of threads must be between 1 and array size.\n");
    //     return 1
    // }

    a = (int*)malloc(a_size * sizeof(int));

    for (int i = 0; i < a_size; i++) {
        a[i] = atoi(argv[i + 3]);
    }

    pthread_mutex_init(&max_mutex, NULL);

    
    pthread_t threads[num_threads];
    ThreadData thread_data[num_threads];

    for (int i = 0; i < num_threads; i++) {
        thread_data[i].start = i * (a_size / num_threads);
        thread_data[i].end = (i == num_threads - 1) ? a_size : (i + 1) * (a_size / num_threads);
        pthread_create(&threads[i], NULL, maximum, &thread_data[i]);
    }

    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    double elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("Maximum Element is : %d\n", max_value);
    cout << fixed << setprecision(6) << "Execution Time: " << elapsed_time << " seconds" << endl;

    free(a);
    pthread_mutex_destroy(&max_mutex);

    return 0;
}
