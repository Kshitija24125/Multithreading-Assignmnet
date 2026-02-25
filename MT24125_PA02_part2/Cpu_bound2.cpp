#include <iostream>
#include <pthread.h>
#include <chrono>
#include <iomanip>
#include <ctime>

using namespace std;
using namespace std::chrono;

const int maxsize = 1000;  // Maximum allowed size of the matrix
int Matrix_A[maxsize][maxsize];
int Matrix_B[maxsize][maxsize];
int Matrix_C[maxsize][maxsize];
int MAX;           // Matrix size (from command line)
int max_threads;   // Number of threads (from command line)

void* Matrix_multiplication(void* arg) {
    int thread_part = *((int*)arg);

    // Each thread processes rows based on its thread_part
    for (int i = thread_part; i < MAX; i += max_threads) {
        for (int j = 0; j < MAX; j++) {
            Matrix_C[i][j] = 0;
            for (int k = 0; k < MAX; k++) {
                Matrix_C[i][j] += Matrix_A[i][k] * Matrix_B[k][j];
            }
        }
    }

    pthread_exit(0);
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " <matrix_size> <num_threads>" << endl;
        return 1;
    }

    MAX = stoi(argv[1]);
    max_threads = stoi(argv[2]);

    
    if (max_threads > MAX) {
        cerr << "Threads count cannot be greater than matrix size." << endl;
        return 1;
    }

    // Initialize matrices A and B with deterministic values
    for (int i = 0; i < MAX; i++) {
        for (int j = 0; j < MAX; j++) {
            Matrix_A[i][j] = (i + 1) * (j + 1);
            Matrix_B[i][j] = (i + j + 2);
        }
    }

    // cout << "Matrix Size: " << MAX << "x" << MAX << ", Threads: " << max_threads << endl;

    // Measure execution time
    // auto start = high_resolution_clock::now();
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    // Create and start threads
    pthread_t threads[max_threads];
    int thread_args[max_threads];
    for (int i = 0; i < max_threads; i++) {
        thread_args[i] = i;
        pthread_create(&threads[i], NULL, Matrix_multiplication, (void*)&thread_args[i]);
    }

    // Join threads
    for (int i = 0; i < max_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    // auto end = high_resolution_clock::now();
    // auto duration = duration_cast<nanoseconds>(end - start);
    clock_gettime(CLOCK_MONOTONIC, &end);
    double elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;



    // for (int i = 0; i < MAX; i++) {
    //     for (int j = 0; j < MAX; j++) {
    //         cout << Matrix_C[i][j] << " ";
    //     }
    //     cout << endl;
    // }

    // Print execution time
    cout << fixed << setprecision(6) << "Execution Time: " << elapsed_time << " seconds" << endl;

    return 0;
}
