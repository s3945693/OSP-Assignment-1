#include <pthread.h>
#include "multithreadshare.h"
#include <list>
#include <cstring>
#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <time.h>

// Declare mutex and condition variable
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t reader_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t writer_cond = PTHREAD_COND_INITIALIZER;
std::deque<std::string> multithreadshare::queue;
std::ifstream multithreadshare::in;
bool multithreadshare::terminateWriters;
std::ofstream multithreadshare::out;
int multithreadshare::count;
double multithreadshare::readerLockTime;
double multithreadshare::writerLockTime;
double multithreadshare::readerWaitTime;
double multithreadshare::writerWaitTime;
double multithreadshare::readerCalled;
double multithreadshare::writerCalled;
std::chrono::high_resolution_clock::time_point 
multithreadshare::startTime;

multithreadshare::multithreadshare(const std::string& inputFile, const std::string& outputFile , const int count ) {
    // Open input file
    in.open(inputFile);
    terminateWriters = false;
    // Open output file
    out.open(outputFile);
    this->count = count;
    readerLockTime = 0;
    writerLockTime = 0;
    readerWaitTime = 0;
    writerWaitTime = 0;
    readerCalled = 0;
    writerCalled = 0;
    startTime = std::chrono::high_resolution_clock::now();
}
void* multithreadshare::readerThread(void* arg) {
    // Code for reader thread
    std::string line;
    size_t buffer_size = 1000000000000;

    while (true) {
        // Lock the mutex
        pthread_mutex_lock(&mutex);
        readerCalled++;
        //clock_t startLock, endLock, startWait, endWait;
        //startLock = clock();
        //startWait = clock();
        auto startLock = std::chrono::high_resolution_clock::now();
        auto startWait = std::chrono::high_resolution_clock::now();
        // Wait for space in buffer
        while (queue.size() >= buffer_size) {
            pthread_cond_wait(&reader_cond, &mutex);
        }
        auto endWait = std::chrono::high_resolution_clock::now();
        readerWaitTime += std::chrono::duration<double>(endWait - startWait).count();

        // Read line from input file and if no lines, turn on termination condition
        if (!std::getline(in, line)) {
            terminateWriters = true;
            pthread_cond_broadcast(&reader_cond);
            auto endLock = std::chrono::high_resolution_clock::now();
            readerLockTime += std::chrono::duration<double>(endLock - startLock).count();
            pthread_mutex_unlock(&mutex);
            break;
        }
        // Add line to queue
        queue.push_back(line);
        // Signal writer thread to write to file
        pthread_cond_signal(&writer_cond);
        // Unlock the mutex
        auto endLock = std::chrono::high_resolution_clock::now();
        readerLockTime += std::chrono::duration<double>(endLock - startLock).count();
        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}

void* multithreadshare::writerThread(void* arg) {
    // Code for writer thread
    std::string line;
    while (true) {
        // Lock the mutex
        pthread_mutex_lock(&mutex);
        writerCalled++;
        //clock_t startLock, endLock, startWait, endWait;
        //startLock = clock();
        //startWait = clock();
        auto startLock = std::chrono::high_resolution_clock::now();
        auto startWait = std::chrono::high_resolution_clock::now();
        //wait for readers to append to queue
        while (queue.empty() && !terminateWriters) {
            pthread_cond_wait(&writer_cond, &mutex);
        }
        auto endWait = std::chrono::high_resolution_clock::now();
        writerWaitTime += std::chrono::duration<double>(endWait - startWait).count();
        // Start to write to queue
        if (!queue.empty()){
            line = queue.front();
            queue.pop_front();
            //check if file already has a line
            if (out.tellp() != 0) {
                out << std::endl;
            }
            out << line;
        }
        //termination condition if queue is empty and readers aren't reading
        else if (terminateWriters){
            //std::cout << pthread_self() << std::endl;
            pthread_cond_broadcast(&writer_cond);
            auto endLock = std::chrono::high_resolution_clock::now();
            writerLockTime += std::chrono::duration<double>(endLock - startLock).count();
            pthread_mutex_unlock(&mutex);
            break;
        }
        // Signal reader thread to read more lines if not eof
        if (!terminateWriters){pthread_cond_signal(&reader_cond);}
        // Unlock the mutex
        auto endLock = std::chrono::high_resolution_clock::now();
        writerLockTime += std::chrono::duration<double>(endLock - startLock).count();
        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}

void multithreadshare::terminateWriterThreads() {
    // Lock the mutex before modifying the termination flag
    pthread_mutex_lock(&mutex);
    terminateWriters = true;
    pthread_cond_broadcast(&writer_cond);
    pthread_mutex_unlock(&mutex);
}

void multithreadshare::run() {
    int numReaders = count;
    int numWriters = count;
    std::vector<pthread_t> readers;
    readers.reserve(numReaders);
    std::vector<pthread_t> writers;  // Use vector for writers
    writers.reserve(numWriters);
    std::cout << "Running multithreadshare" << std::endl;

    for (int i = 0; i < numReaders; i++) {
        pthread_t thread;
        int err = pthread_create(&thread, NULL, readerThread, NULL);
        while(err!=0){err = pthread_create(&thread, NULL, writerThread, NULL);}
        if (err != 0) {
            std::cerr << "Failed to create reader thread: " << strerror(err) << std::endl;
            exit(EXIT_FAILURE);
        }
        readers.push_back(thread);
    }
    for (int i = 0; i < numWriters; i++) {
        //usleep(1);

        //std::cout << i <<" "<< queue.size()<<std::endl;
        pthread_t thread;
        
        int err = pthread_create(&thread, NULL, writerThread, NULL);  // Use writers.at(i)
        while(err!=0){err = pthread_create(&thread, NULL, writerThread, NULL);}
        if (err != 0) {
            std::cerr << "Failed to create writer thread: " << strerror(err) << std::endl;
            exit(EXIT_FAILURE);
        }
        writers.push_back(thread);
    }
    std::cout << "joining read" << std::endl;
    for (pthread_t thread : readers) {
        pthread_join(thread, NULL);
    }
    std::cout << "fin read" << std::endl;
    terminateWriterThreads();
    std::cout << "joining write" << std::endl;
    for (int i = 0; i < numWriters; i++) {
        pthread_join(writers.at(i), NULL);  // Use writers.at(i)
        //std::cout << "Finished joining writer: " << i << std::endl;
    }
    auto endTime = std::chrono::high_resolution_clock::now();
    double totalRuntime = std::chrono::duration<double>(endTime - startTime).count();

    // Print results
    std::cout << "Total runtime: " << totalRuntime << " seconds" << std::endl;
    std::cout << "Total/Averayge Reader lock time: " << readerLockTime << " / " << (readerLockTime / readerCalled) << " seconds" << std::endl;
    std::cout << "Total/Average Reader wait time: " << readerLockTime << " / " << (readerWaitTime / readerCalled) << " seconds" << std::endl;
    std::cout << "Total/Average Writer lock time: " << writerWaitTime << " / " << (writerLockTime / writerCalled) << " seconds" << std::endl;
    std::cout << "Total/Average Writer wait time: " << writerWaitTime << " / " << (writerWaitTime / writerCalled) << " seconds" << std::endl;
    std::cout << "Average Reader called: " << readerCalled << std::endl;
    std::cout << "Average Writer called: " << writerCalled << std::endl;


    in.close();
    out.close();

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&reader_cond);
    pthread_cond_destroy(&writer_cond);
}
