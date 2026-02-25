// #include <pthread.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <time.h>
// #include<iostream>
// #include<chrono>
// int MATRIX_SIZE ;
// char*  FILE_NAME ;
// using namespace std;


// pthread_mutex_t file_mutex = PTHREAD_MUTEX_INITIALIZER;
// pthread_cond_t matrix_ready = PTHREAD_COND_INITIALIZER;
// pthread_cond_t processing_done = PTHREAD_COND_INITIALIZER;

// int matrix_computed = 0;
// int processing_completed = 0;

// void* cpu_bound_task(void* arg);
// void* memory_bound_task(void* arg);
// void* io_bound_task(void* arg);

// void* cpu_bound_task(void* arg) {

//     int matrixA[MATRIX_SIZE][MATRIX_SIZE], matrixB[MATRIX_SIZE][MATRIX_SIZE], matrixC[MATRIX_SIZE][MATRIX_SIZE];
//     srand(time(NULL));

//     for (int i = 0; i < MATRIX_SIZE; i++) {
//         for (int j = 0; j < MATRIX_SIZE; j++) {
//             matrixA[i][j] = rand() % 100;
//             matrixB[i][j] = rand() % 100;
//             matrixC[i][j] = 0;
//         }
//     }

//     for (int i = 0; i < MATRIX_SIZE; i++) {
//         for (int j = 0; j < MATRIX_SIZE; j++) {
//             for (int k = 0; k < MATRIX_SIZE; k++) {
//                 matrixC[i][j] += matrixA[i][k] * matrixB[k][j];
//             }
//         }
//     }

//     // Write the result matrix to a file
//     pthread_mutex_lock(&file_mutex);
//     FILE* file = fopen(FILE_NAME, "w");
//     if (file == NULL) {
//         perror("Error opening file");
//         pthread_mutex_unlock(&file_mutex);
//         pthread_exit(NULL);
//     }

//     for (int i = 0; i < MATRIX_SIZE; i++) {
//         for (int j = 0; j < MATRIX_SIZE; j++) {
//             fprintf(file, "%d ", matrixC[i][j]);
//         }
//         fprintf(file, "\n");
//     }
//     fclose(file);

//     matrix_computed = 1;
//     pthread_cond_signal(&matrix_ready);
//     pthread_mutex_unlock(&file_mutex);
//     pthread_exit(NULL);
// }

// void* memory_bound_task(void* arg) {
//     pthread_mutex_lock(&file_mutex);
//     while (!matrix_computed) {
//         pthread_cond_wait(&matrix_ready, &file_mutex);
//     }


//     FILE* file = fopen(FILE_NAME, "r");
//     if (file == NULL) {
//         perror("Error opening file");
//         pthread_mutex_unlock(&file_mutex);
//         pthread_exit(NULL);
//     }

//     int number, largest = 0;
//     while (fscanf(file, "%d", &number) == 1) {
//         if (number > largest) {
//             largest = number;
//         }
//     }
//     fclose(file);

//     processing_completed = 1;
//     pthread_cond_signal(&processing_done);
//     pthread_mutex_unlock(&file_mutex);

//     cout << "Memory-bound task completed. Largest number = " << largest<<endl;
//     return NULL;
// }

// void* io_bound_task(void* arg) {
//     pthread_mutex_lock(&file_mutex);
//     while (!processing_completed) {
//         pthread_cond_wait(&processing_done, &file_mutex);
//     }

//     printf("I/O-bound task: Reading and displaying the matrix result...\n");

//     FILE* file = fopen(FILE_NAME, "r");
//     if (file == NULL) {
//         perror("Error opening file");
//         pthread_mutex_unlock(&file_mutex);
//         pthread_exit(NULL);
//     }

//     char line[10000];
//     while (fgets(line, sizeof(line), file) != NULL) {
//         printf("%s", line);
//     }

//     fclose(file);
//     pthread_mutex_unlock(&file_mutex);
//     pthread_exit(NULL);
// }

// int main(int argc, char* argv[]) {
//     struct timespec start, end;
//     clock_gettime(CLOCK_MONOTONIC, &start);
//     if (argc != 3) {
//         fprintf(stderr, "Usage: %s <matrix_size> <output_file>\n", argv[0]);
//         return 1;
//     }

//     MATRIX_SIZE = atoi(argv[1]);
    
//     FILE_NAME = argv[2];
//     pthread_t cpu_thread, mem_thread, io_thread;

//     // Create and run all tasks concurrently
//     pthread_create(&cpu_thread, NULL, cpu_bound_task, NULL);
//     pthread_create(&mem_thread, NULL, memory_bound_task, NULL);
//     pthread_create(&io_thread, NULL, io_bound_task, NULL);

//     // Wait for all tasks to complete
//     pthread_join(cpu_thread, NULL);
//     pthread_join(mem_thread, NULL);
//     pthread_join(io_thread, NULL);

//     clock_gettime(CLOCK_MONOTONIC, &end);
//     double elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;


//     cout << "Execution Time:"<< elapsed_time << " seconds\n";
//     pthread_mutex_destroy(&file_mutex);
//     pthread_cond_destroy(&matrix_ready);
//     pthread_cond_destroy(&processing_done);

//     return 0;
// }
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <chrono>
#include <vector>
#include <iomanip>
using namespace std;

int MATRIX_SIZE;
char* FILE_NAME;
int NUM_THREADS;

pthread_mutex_t file_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t matrix_ready = PTHREAD_COND_INITIALIZER;
pthread_cond_t processing_done = PTHREAD_COND_INITIALIZER;

int matrix_computed = 0;
int processing_completed = 0;

struct ThreadData {
    int startRow;
    int endRow;
    int** matrixA;
    int** matrixB;
    int** matrixC;
};

void* cpu_bound_task(void* arg);
void* memory_bound_task(void* arg);
void* io_bound_task(void* arg);

int** allocate_matrix(int size) {
    int** matrix = new int*[size];
    for (int i = 0; i < size; ++i) {
        matrix[i] = new int[size];
    }
    return matrix;
}

void free_matrix(int** matrix, int size) {
    for (int i = 0; i < size; ++i) {
        delete[] matrix[i];
    }
    delete[] matrix;
}

void* cpu_bound_task(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    int startRow = data->startRow;
    int endRow = data->endRow;

    for (int i = startRow; i <= endRow; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            data->matrixC[i][j] = 0;
            for (int k = 0; k < MATRIX_SIZE; k++) {
                data->matrixC[i][j] += data->matrixA[i][k] * data->matrixB[k][j];
            }
        }
    }

    pthread_mutex_lock(&file_mutex);
    if (endRow == MATRIX_SIZE - 1) {
        matrix_computed = 1;
        pthread_cond_signal(&matrix_ready);
    }
    pthread_mutex_unlock(&file_mutex);

    pthread_exit(NULL);
}

void* memory_bound_task(void* arg) {
    pthread_mutex_lock(&file_mutex);
    while (!matrix_computed) {
        pthread_cond_wait(&matrix_ready, &file_mutex);
    }

    FILE* file = fopen(FILE_NAME, "w");
    if (file == NULL) {
        perror("Error opening file");
        pthread_mutex_unlock(&file_mutex);
        pthread_exit(NULL);
    }

    int** matrixC = ((ThreadData*)arg)->matrixC;
    int largest = 0;
    for (int i = 0; i < MATRIX_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            fprintf(file, "%d ", matrixC[i][j]);
            if (matrixC[i][j] > largest) {
                largest = matrixC[i][j];
            }
        }
        fprintf(file, "\n");
    }
    fclose(file);

    processing_completed = 1;
    pthread_cond_signal(&processing_done);
    pthread_mutex_unlock(&file_mutex);

    cout << "Memory-bound task completed. Largest number = " << largest << endl;
    pthread_exit(NULL);
}

void* io_bound_task(void* arg) {
    pthread_mutex_lock(&file_mutex);
    while (!processing_completed) {
        pthread_cond_wait(&processing_done, &file_mutex);
    }

    printf("I/O-bound task: Reading and displaying the matrix result...\n");
    FILE* file = fopen(FILE_NAME, "r");
    if (file == NULL) {
        perror("Error opening file");
        pthread_mutex_unlock(&file_mutex);
        pthread_exit(NULL);
    }

    char line[1024];
    while (fgets(line, sizeof(line), file) != NULL) {
        printf("%s", line);
    }

    fclose(file);
    pthread_mutex_unlock(&file_mutex);
    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <matrix_size> <output_file> <num_threads>\n", argv[0]);
        return 1;
    }
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    MATRIX_SIZE = atoi(argv[1]);
    FILE_NAME = argv[2];
    NUM_THREADS = atoi(argv[3]);

    int** matrixA = allocate_matrix(MATRIX_SIZE);
    int** matrixB = allocate_matrix(MATRIX_SIZE);
    int** matrixC = allocate_matrix(MATRIX_SIZE);

    srand(time(NULL));
    for (int i = 0; i < MATRIX_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            matrixA[i][j] = rand() % 100;
            matrixB[i][j] = rand() % 100;
        }
    }

    vector<pthread_t> threads(NUM_THREADS);
    vector<ThreadData> threadData(NUM_THREADS);
    int rows_per_thread = MATRIX_SIZE / NUM_THREADS;

    for (int i = 0; i < NUM_THREADS; i++) {
        threadData[i].startRow = i * rows_per_thread;
        threadData[i].endRow = (i + 1) * rows_per_thread - 1;
        if (i == NUM_THREADS - 1) threadData[i].endRow = MATRIX_SIZE - 1;

        threadData[i].matrixA = matrixA;
        threadData[i].matrixB = matrixB;
        threadData[i].matrixC = matrixC;

        pthread_create(&threads[i], NULL, cpu_bound_task, &threadData[i]);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_t mem_thread, io_thread;
    pthread_create(&mem_thread, NULL, memory_bound_task, &threadData[0]);
    pthread_create(&io_thread, NULL, io_bound_task, NULL);

    pthread_join(mem_thread, NULL);
    pthread_join(io_thread, NULL);

    free_matrix(matrixA, MATRIX_SIZE);
    free_matrix(matrixB, MATRIX_SIZE);
    free_matrix(matrixC, MATRIX_SIZE);
    clock_gettime(CLOCK_MONOTONIC, &end);
    double elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    cout << fixed << setprecision(6) << "Execution Time: " << elapsed_time << " seconds" << endl;

    pthread_mutex_destroy(&file_mutex);
    pthread_cond_destroy(&matrix_ready);
    pthread_cond_destroy(&processing_done);

    return 0;
}
