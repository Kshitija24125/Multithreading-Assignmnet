#include <iostream>
#include <fstream>
#include <pthread.h>
#include <vector>
#include <string>
#include <mutex>
#include <unistd.h>  
#include <cstdlib>   
#include<chrono>
#include<iomanip>
using namespace std;

mutex file_mutex;        
const string filename = "numbers.txt";
bool stop_threads = false;
int num_writer_threads = 0;   // Global variable to store the number of threads

// Function to read all numbers from the file
vector<int> read_file() {
    vector<int> numbers;
    ifstream infile(filename);

    if (infile.is_open()) {
        string line;
        while (getline(infile, line)) {
            try {
                int number = std::stoi(line);
                numbers.push_back(number);
            } catch (...) {
                cerr << "Invalid input detected.\n";
            }
        }
        infile.close();
    } else {
        cerr << "Unable to open file for reading.\n";
    }

    return numbers;
}

// Function to write specific elements to the file
void write_file(const vector<int>& numbers) {
    ofstream outfile(filename, ios::trunc);

    if (outfile.is_open()) {
        for (int number : numbers) {
            outfile << number << "\n";
        }
        outfile.close();
    } else {
        cerr << "Unable to open file for writing.\n";
    }
}

// Function for writer threads to multiply their assigned elements by 2
void* writer_thread(void* arg) {

    int thread_id = *static_cast<int*>(arg);

    // Read the file and get the data
    vector<int> numbers;
    {
        lock_guard<std::mutex> lock(file_mutex);
        numbers = read_file();
    }

    int total_elements = numbers.size();
    int elements_per_thread = total_elements / num_writer_threads;
    int start_index = thread_id * elements_per_thread;
    int end_index = (thread_id == num_writer_threads - 1) ? total_elements : start_index + elements_per_thread;

    for (int i = start_index; i < end_index; ++i) {
        numbers[i] *= 2;
    }

    // Write the updated portion back to the file
    {
        lock_guard<mutex> lock(file_mutex);
        write_file(numbers);
    }

    return nullptr;
}

// Function for the reader thread to read and print the file contents
void* reader_thread(void* arg) {
    while (!stop_threads) {
        {
            lock_guard<mutex> lock(file_mutex);
            ifstream infile(filename);

            if (infile.is_open()) {
                cout << "Reader: Reading file contents...\n";
                string line;
                while (getline(infile, line)) {
                cout << "Reader: " << line << "\n";
                }
            } else {
                cerr << "Reader: Unable to open file for reading.\n";
            }
        }

        usleep(500000);  
    }

    cout << "Reader: Stopping.\n";
    return nullptr;
}

int main(int argc, char* argv[]) {
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <num_writer_threads>\n";
        return 1;
    }

    num_writer_threads = std::stoi(argv[1]);
    if (num_writer_threads <= 0) {
        cerr << "Error: Number of writer threads must be a positive integer.\n";
        return 1;
    }

    // Start the single reader thread
    pthread_t reader;
    if (pthread_create(&reader, nullptr, reader_thread, nullptr) != 0) {
        cerr << "Error: Failed to create reader thread.\n";
        return 1;
    }

    // Start writer threads
    vector<pthread_t> writer_threads(num_writer_threads);
    vector<int> thread_ids(num_writer_threads);

    for (int i = 0; i < num_writer_threads; ++i) {
        thread_ids[i] = i;
        if (pthread_create(&writer_threads[i], nullptr, writer_thread, &thread_ids[i]) != 0) {
            cerr << "Error: Failed to create writer thread " << i << ".\n";
            return 1;
        }
    }

    // Wait for all writer threads to finish
    for (int i = 0; i < num_writer_threads; ++i) {
        if (pthread_join(writer_threads[i], nullptr) != 0) {
            cerr << "Error: Failed to join writer thread " << i << ".\n";
            return 1;
        }
    }

    // Stop the reader thread after writers are done
    stop_threads = true;
    if (pthread_join(reader, nullptr) != 0) {
        cerr << "Error: Failed to join reader thread.\n";
        return 1;
    }
   clock_gettime(CLOCK_MONOTONIC, &end);
   double elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;


    cout << fixed << setprecision(6) << "Execution Time: " << elapsed_time << " seconds" << endl;
    return 0;
}
