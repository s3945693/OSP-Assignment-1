#include <pthread.h>
#include "multithreadshare.h"
// Declare mutex and condition variable
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
std::queue<std::string> multithreadshare::queue;



multithreadshare::multithreadshare(const std::string& inputFile, const std::string& outputFile , const int count ) {
    // Open input file
    in.open(inputFile);
    terminateWriters = false;
    // Open output file
    out.open(outputFile);
    this->count = count;
}

void* multithreadshare::readerThread(void* arg) {
    // Code for reader thread
    std::string line;

    while (true) {
        // Lock the mutex
        pthread_mutex_lock(&mutex);

        // Read line from input file
        if (!std::getline(in, line)) {
            std::cout<< "Finished reading file" << std::endl;
            pthread_mutex_unlock(&mutex);
            break;
        }

        // Add line to queue
        queue.push(line);

        // Signal writer thread
        pthread_cond_signal(&cond);

        // Unlock the mutex
        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}

void* multithreadshare::writerThread(void* arg) {
    // Code for writer thread
    std::string line;

    while (!queue.empty()) {
        // Lock the mutex
        pthread_mutex_lock(&mutex);

        // Wait for line in queue or eof flag to be set
        while (queue.empty() && !terminateWriters) {
            pthread_cond_wait(&cond, &mutex);
        }

        // Check if the queue is empty and the terminateWriters flag is set
        if (queue.empty() && terminateWriters) {
            pthread_mutex_unlock(&mutex);
            std::cout << "file has been written" << std::endl;
            break;
        }

        // Remove line from queue
        line = queue.front();
        queue.pop();

        // Write line to output file
        out << line; out<<std::endl;

        // Unlock the mutex
        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}

void multithreadshare::terminateWriterThreads() {
    // Lock the mutex before modifying the termination flag
    pthread_mutex_lock(&mutex);

    // Wait for the queue to be empty
    

    terminateWriters = true;
    // Signal the condition variable to wake up waiting writer threads
    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&mutex);
}



static void* readerThreadWrapper(void* arg) {
        multithreadshare* obj = static_cast<multithreadshare*>(arg);
        return obj->readerThread(NULL);
    }

static void* writerThreadWrapper(void* arg) {
    multithreadshare* obj = static_cast<multithreadshare*>(arg);
    return obj->writerThread(NULL);
}

void multithreadshare::run() {
    int numReaders = count;
    int numWriters = count;
    std::vector<pthread_t>* readers = new std::vector<pthread_t>(numReaders);
    std::vector<pthread_t>* writers = new std::vector<pthread_t>(numWriters);

    std::cout << "Running multithreadshare" << std::endl;
    // Create reader threads
    for (int i = 0; i < numReaders; i++) {
        pthread_create(&(*readers)[i], NULL, readerThreadWrapper, this);
    }

    // Create writer threads
    for (int i = 0; i < numWriters; i++) {
        pthread_create(&(*writers)[i], NULL, writerThreadWrapper, this);
    }

    std::cout << "Finshied creatin threads" << std::endl;
    // Wait for all threads to finish
    for (int i = 0; i < numReaders; i++) {
        pthread_join((*readers)[i], NULL);
    }
    terminateWriterThreads();

    std::cout << numWriters << std::endl;
    for (int i = 0; i < numWriters; i++) {
        std::cout << "at thread" << i << (*writers)[i];
        pthread_join((*writers)[i], NULL);
        std::cout<< ": Done" << std::endl;
    }
    std::cout << "Finshied write threads" << std::endl;
    in.close();
    out.close();
    delete readers;
    delete writers;
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
}
