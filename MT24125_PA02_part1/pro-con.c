
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define TOTAL_ITEMS 100  

typedef struct {
    int *buffer;           
    int in, out;           
    int bufferSize;        
    int itemsProduced;     
    int itemsConsumed;     
    pthread_mutex_t mutex; 
    pthread_mutex_t buffer_mutex;  
    pthread_cond_t buffer_not_full;  
    pthread_cond_t buffer_not_empty; 
} SharedBuffer;

// Initialize the buffer (buffer size = N)
void initBuffer(SharedBuffer *shared, int bufferSize) {
    shared->bufferSize = bufferSize;  // Buffer size equal to N
    shared->buffer = (int *)malloc(shared->bufferSize * sizeof(int));
    shared->in = shared->out = 0;
    shared->itemsProduced = 0;
    shared->itemsConsumed = 0;
    pthread_mutex_init(&shared->mutex, NULL);
    pthread_mutex_init(&shared->buffer_mutex, NULL);
    pthread_cond_init(&shared->buffer_not_full, NULL);
    pthread_cond_init(&shared->buffer_not_empty, NULL);
}

// Destroy the buffer
void destroyBuffer(SharedBuffer *shared) {
    free(shared->buffer);
    pthread_mutex_destroy(&shared->mutex);
    pthread_mutex_destroy(&shared->buffer_mutex);
    pthread_cond_destroy(&shared->buffer_not_full);
    pthread_cond_destroy(&shared->buffer_not_empty);
}

// Generate random number
int generateRandomNumber() {
    return rand() % 100 + 1;
}

// Producer Function
void *producer(void *arg) {
    SharedBuffer *shared = (SharedBuffer *)arg;

    while (1) {
        pthread_mutex_lock(&shared->mutex);
        if (shared->itemsProduced >= TOTAL_ITEMS) { 
            pthread_mutex_unlock(&shared->mutex);
            break;
        }
        int num = generateRandomNumber();
        shared->itemsProduced++;
        pthread_mutex_unlock(&shared->mutex);

        pthread_mutex_lock(&shared->buffer_mutex);
        
        // Wait if buffer is full
        while ((shared->in + 1) % shared->bufferSize == shared->out) {
            pthread_cond_wait(&shared->buffer_not_full, &shared->buffer_mutex);
        }
        
        shared->buffer[shared->in] = num;
        printf("Producer %lu produced: %d\n", pthread_self(), num);
        shared->in = (shared->in + 1) % shared->bufferSize;

        pthread_cond_signal(&shared->buffer_not_empty); // Wake up consumers
        pthread_mutex_unlock(&shared->buffer_mutex);
        
        usleep(50);  // Introduce small delay to allow organic context switching
    }
    pthread_exit(NULL);
}

// Consumer Function
void *consumer(void *arg) {
    SharedBuffer *shared = (SharedBuffer *)arg;
    int id = pthread_self() % shared->bufferSize; 
    int cumulativeSum = 0;

    int itemsToConsume = TOTAL_ITEMS / shared->bufferSize;

    for (int i = 0; i < itemsToConsume; i++) {
        pthread_mutex_lock(&shared->buffer_mutex);
        
        // Wait if buffer is empty
        while (shared->in == shared->out) {
            pthread_cond_wait(&shared->buffer_not_empty, &shared->buffer_mutex);
        }
        
        int num = shared->buffer[shared->out];
        shared->out = (shared->out + 1) % shared->bufferSize;

        pthread_cond_signal(&shared->buffer_not_full); // Wake up producers
        pthread_mutex_unlock(&shared->buffer_mutex);

        pthread_mutex_lock(&shared->mutex);
        shared->itemsConsumed++;
        pthread_mutex_unlock(&shared->mutex);

        printf("Consumer %d consumed: %d\n", id, num);
        cumulativeSum += num;

        usleep(50);  // Introduce small delay to allow organic context switching
    }

    double average = (double)cumulativeSum / itemsToConsume;
    printf("Consumer %d processed %d numbers. Cumulative Sum: %d, Average: %.2f\n",
           id, itemsToConsume, cumulativeSum, average);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <num_producer_consumer_pairs>\n", argv[0]);
        return 1;
    }

    int N = atoi(argv[1]); 

    if (N <= 0) {
        fprintf(stderr, "Error: Number of producer-consumer pairs must be greater than 0.\n");
        return 1;
    }

    printf("Starting with %d producer-consumer pairs. Processing exactly %d numbers.\n",
           N, TOTAL_ITEMS);

    pthread_t producers[N], consumers[N];
    SharedBuffer shared;
    initBuffer(&shared, N);  // Keep buffer size = N

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start); 

    // Start producer and consumer threads
    for (int i = 0; i < N; i++) {
        pthread_create(&producers[i], NULL, producer, (void *)&shared);
        pthread_create(&consumers[i], NULL, consumer, (void *)&shared);
    }

    // Wait for all threads to complete
    for (int i = 0; i < N; i++) {
        pthread_join(producers[i], NULL);
        pthread_join(consumers[i], NULL);
    }

    clock_gettime(CLOCK_MONOTONIC, &end); 

    // Calculate execution time
    double executionTime = (end.tv_sec - start.tv_sec) * 1000.0 + (end.tv_nsec - start.tv_nsec) / 1e6;

    printf("\nPerformance Results\n");
    printf("Total execution time: %.2f ms\n", executionTime);
    printf("Total items processed: %d\n", TOTAL_ITEMS);
    printf("Throughput: %.2f items per second\n", TOTAL_ITEMS / (executionTime / 1000.0));

    destroyBuffer(&shared);
    return 0;
}
