#include <pthread.h>
#include "multithreadshare.h"
#include <list>

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
    //std::cout <<"queue exists" << std::endl;
    while (!queue.empty()) {
        // Lock the mutex
        //std::cout << "mutex lock" <<std::endl;
        pthread_mutex_lock(&mutex);
        //std::cout << "after lock" <<std::endl;
        // Wait for line in queue or eof flag to be set
        while (queue.empty() && !terminateWriters) {
            //std::cout <<"waiting line" <<std::endl;
            pthread_cond_wait(&cond, &mutex);
        }
        //std::cout << "b4 break" <<std::endl;
        // Check if the queue is empty and the terminateWriters flag is set
        if (queue.empty() && terminateWriters) {
            //std::cout <<"terminate line" <<std::endl;
            pthread_mutex_unlock(&mutex);
            break;
        }

        // Remove line from queue
        //std::cout << "b4 write" <<std::endl;
        if(!queue.empty()){
            //std::cout <<"writing line" <<std::endl;
            line = queue.front();
            queue.pop();

            // Write line to output file
            out << line; if(!queue.empty()){out<<std::endl;}
        }

        // Unlock the mutex
        //std::cout << "unlock lock" <<std::endl;
        pthread_mutex_unlock(&mutex);
    }
    //std::cout << "null" <<std::endl;
    return NULL;
}

void multithreadshare::terminateWriterThreads() {
    // Lock the mutex before modifying the termination flag
    pthread_mutex_lock(&mutex);
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


///*
void multithreadshare::run() {
    int numReaders = count;
    int numWriters = count;
    std::vector<pthread_t*> readers;
    std::vector<pthread_t*> writers;
    std::cout << "Running multithreadshare" << std::endl;
    
    // Create reader threads
    for (int i = 0; i < numReaders; i++) {
        pthread_t* thread = new pthread_t;
        pthread_create(thread, NULL, readerThreadWrapper, this);
        readers.push_back(thread);
    }

    

    std::cout << "Finished creating threads" << std::endl;

    // Wait for all threads to finish
    for (pthread_t* thread : readers) {
        pthread_join(*thread, NULL);
        delete thread;
    }
    std::cout << "Finished joining read threads" << std::endl;
    terminateWriterThreads();
    // Create writer threads
    for (int i = 0; i < numWriters; i++) {
        pthread_t* thread = new pthread_t;
        pthread_create(thread, NULL, writerThreadWrapper, this);
        writers.push_back(thread);
    }
    
    for (pthread_t* thread : writers) {
        pthread_join(*thread, NULL);
        delete thread;
    }
    std::cout << "Finished writer threads" << std::endl;

    in.close();
    out.close();

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
}
//*/