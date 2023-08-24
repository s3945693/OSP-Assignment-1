#include <pthread.h>
#include "multithreadshare.h"
#include <list>
#include <cstring>
#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <time.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t reader_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t writer_cond = PTHREAD_COND_INITIALIZER;

//for some reason the file would not compile without these declerations
std::deque<std::vector<char>> multithreadshare::queue;
std::deque<std::vector<char>>::size_type buffer_size = 100000000;

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
multithreadshare::start;

multithreadshare::multithreadshare(const std::string& inputFile, const std::string& outputFile , const int count ) {
    
    in.open(inputFile);
    terminateWriters = false;
    out.open(outputFile);
    
    this->count = count;
    readerLockTime = 0;
    writerLockTime = 0;
    readerWaitTime = 0;
    writerWaitTime = 0;
    readerCalled = 0;
    writerCalled = 0;
    start = std::chrono::high_resolution_clock::now();
}
void* multithreadshare::readerThread(void* arg) {
    //buffer for reader
    std::vector<char> buffer(50000); 

    while (true) {
        
        auto startLock = std::chrono::high_resolution_clock::now();
        pthread_mutex_lock(&mutex);
        readerCalled++;

        auto startWait = std::chrono::high_resolution_clock::now();
        //condition to wait for space
        while (queue.size() >= buffer_size) {
            pthread_cond_wait(&reader_cond, &mutex);
        }
        auto endWait = std::chrono::high_resolution_clock::now();
        readerWaitTime += std::chrono::duration<double>(endWait - startWait).count();

        in.read(buffer.data(), buffer.size());
        size_t bytesRead = in.gcount();

        //if no bytes read, turn on termination condition
        if (bytesRead == 0) {
            terminateWriters = true;
            pthread_cond_broadcast(&reader_cond);
            pthread_mutex_unlock(&mutex);
            auto endLock = std::chrono::high_resolution_clock::now();
            readerLockTime += std::chrono::duration<double>(endLock - startLock).count();
            break;
        }

        //add read bytes to the queue
        queue.emplace_back(buffer.begin(), buffer.begin() + bytesRead);
        
        //signal writer thread to write to file
        pthread_cond_signal(&writer_cond);
        pthread_mutex_unlock(&mutex);
        auto endLock = std::chrono::high_resolution_clock::now();
        readerLockTime += std::chrono::duration<double>(endLock - startLock).count();
    }

    return NULL;
}

void* multithreadshare::writerThread(void* arg) {
    while (true) {

        auto startLock = std::chrono::high_resolution_clock::now();
        pthread_mutex_lock(&mutex);
        writerCalled++;

        auto startWait = std::chrono::high_resolution_clock::now();
        //wait for bytes from readers
        while (queue.empty() && !terminateWriters) {
            pthread_cond_wait(&writer_cond, &mutex);
        }
        auto endWait = std::chrono::high_resolution_clock::now();
        writerWaitTime += std::chrono::duration<double>(endWait - startWait).count();

        if (!queue.empty()) {
            std::vector<char> bytesToWrite = queue.front();
            queue.pop_front();

            out.write(bytesToWrite.data(), bytesToWrite.size());
            out.flush();

            if (!terminateWriters) {
                pthread_cond_signal(&reader_cond);
            }
        } 

        //if file has been fully read, signal writers to finish their job
        else if (terminateWriters) {
            pthread_cond_broadcast(&writer_cond);
            pthread_mutex_unlock(&mutex);
            auto endLock = std::chrono::high_resolution_clock::now();
            writerLockTime += std::chrono::duration<double>(endLock - startLock).count();
            break;
        }

        pthread_mutex_unlock(&mutex);
        auto endLock = std::chrono::high_resolution_clock::now();
        writerLockTime += std::chrono::duration<double>(endLock - startLock).count();
    }

    return NULL;
}


void multithreadshare::terminateWriterThreads() {
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
    std::vector<pthread_t> writers; 
    writers.reserve(numWriters);
    std::cout << "Running multithreadshare" << std::endl;
    //ran into errors while creating threads, fix was to loop till thread was created
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
        
        int err = pthread_create(&thread, NULL, writerThread, NULL); 
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
        pthread_join(writers.at(i), NULL); 
        //std::cout << "Finished joining writer: " << i << std::endl;
    }
    auto end = std::chrono::high_resolution_clock::now();
    double total = std::chrono::duration<double>(end - start).count();

    std::cout << "Total runtime: " << total << " seconds" << std::endl;
    std::cout << "Total/Average Reader lock time: " << readerLockTime << " / " << (readerLockTime / readerCalled) << " seconds" << std::endl;
    std::cout << "Total/Average Reader wait time: " << readerWaitTime << " / " << (readerWaitTime / readerCalled) << " seconds" << std::endl;
    std::cout << "Total/Average Writer lock time: " << writerLockTime << " / " << (writerLockTime / writerCalled) << " seconds" << std::endl;
    std::cout << "Total/Average Writer wait time: " << writerWaitTime << " / " << (writerWaitTime / writerCalled) << " seconds" << std::endl;
    std::cout << "Average Reader called: " << readerCalled << std::endl;
    std::cout << "Average Writer called: " << writerCalled << std::endl;


    in.close();
    out.close();

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&reader_cond);
    pthread_cond_destroy(&writer_cond);
}
