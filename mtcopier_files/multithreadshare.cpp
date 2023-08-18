#include <pthread.h>
#include "multithreadshare.h"
#include <list>

// Declare mutex and condition variable
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t reader_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t writer_cond = PTHREAD_COND_INITIALIZER;
std::deque<std::string> multithreadshare::queue;


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
    size_t buffer_size = 10;

    while (true) {
        // Lock the mutex
        pthread_mutex_lock(&mutex);

        // Wait for space in buffer
        while (queue.size() >= buffer_size) {
            pthread_cond_wait(&reader_cond, &mutex);
        }

        // Read line from input file
        if (!std::getline(in, line)) {
            pthread_mutex_unlock(&mutex);
            if (!terminateWriters){
                terminateWriterThreads();
                std::cout<<"ran" << std::endl;
            }
            break;
        }

        // Add line to buffer
        queue.push_back(line);

        // Signal writer thread
        pthread_cond_signal(&writer_cond);

        // Unlock the mutex
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

        // Wait for line in queue or eof flag to be set
        while (queue.empty() && !terminateWriters) {
            pthread_cond_wait(&writer_cond, &mutex);
        }

        // Check if the queue is empty and the terminateWriters flag is set
        if (queue.empty() && terminateWriters) {
            pthread_mutex_unlock(&mutex);
            break;
        }

        // Remove line from queue
        line = queue.front();
        queue.pop_front();

        // Signal reader thread
        pthread_cond_signal(&reader_cond);

        // Write line to output file
        out << line; 
        if(!terminateWriters){out<<std::endl;}
        else{if(!queue.empty()){out<<std::endl;}}

        // Unlock the mutex
        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}


void multithreadshare::terminateWriterThreads() {
    // Lock the mutex before modifying the termination flag
    pthread_mutex_lock(&mutex);
    terminateWriters = true;
    // Signal the condition variable to wake up waiting writer threads
    pthread_cond_broadcast(&writer_cond);
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

    clock_t start, end;
    start = clock();
    // Create reader threads
    for (int i = 0; i < numReaders; i++) {
        pthread_t* thread = new pthread_t;
        pthread_create(thread, NULL, readerThreadWrapper, this);
        readers.push_back(thread);
    }
    end = clock();    
    double seconds = (double)(end - start) / CLOCKS_PER_SEC;
    std::cout << "Finished creating threads" << seconds << std::endl;
    start = clock();
    // Create writer threads
    for (int i = 0; i < numWriters; i++) {
        pthread_t* thread = new pthread_t;
        pthread_create(thread, NULL, writerThreadWrapper, this);
        writers.push_back(thread);
    }
    end = clock();
    seconds = (double)(end - start) / CLOCKS_PER_SEC;
    std::cout<< "Finished creating writer threads" << seconds << std::endl;
    // Wait for all threads to finish
    start = clock();
    for (pthread_t* thread : readers) {
        pthread_join(*thread, NULL);
        delete thread;
    }
    end = clock();
    seconds = (double)(end - start) / CLOCKS_PER_SEC;
    std::cout << "Finished joining read threads" << seconds << std::endl;

    start = clock();
    for (pthread_t* thread : writers) {
        pthread_join(*thread, NULL);
        delete thread;
    }

    end = clock();
    seconds = (double)(end - start) / CLOCKS_PER_SEC;
    std::cout << "Finished writer threads" << seconds << std::endl;

    in.close();
    out.close();

    // Destroy mutex and condition variables
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&reader_cond);
    pthread_cond_destroy(&writer_cond);
}

//*/