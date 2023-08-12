#include <pthread.h>
#include "multithreadshare.h"
// Declare mutex and condition variable
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
std::queue<std::string> multithreadshare::queue;

multithreadshare::multithreadshare(const std::string& inputFile, const std::string& outputFile , const int count ) {
    // Open input file
    in.open(inputFile);

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
            // End of file reached
            // Add end-of-file marker to queue
            queue.push("EOF");
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

    while (true) {
        // Lock the mutex
        pthread_mutex_lock(&mutex);

        // Wait for line in queue
        while (queue.empty()) {
            pthread_cond_wait(&cond, &mutex);
        }

        // Remove line from queue
        line = queue.front();
        queue.pop();

        // Check for end-of-file marker
        if (line == "EOF") {
            pthread_mutex_unlock(&mutex);
            break;
        }

        // Write line to output file
        out << line << std::endl;

        // Unlock the mutex
        pthread_mutex_unlock(&mutex);
    }

    return NULL;
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
    int numReaders = 5;
    int numWriters = 5;
    std::vector<pthread_t> readers(numReaders);
    std::vector<pthread_t> writers(numWriters);
    std::cout << "Running multithreadshare" << std::endl;
    // Create reader threads
    for (int i = 0; i < numReaders; i++) {
        pthread_create(&readers[i], NULL, readerThreadWrapper, this);
    }

    // Create writer threads
    for (int i = 0; i < numWriters; i++) {
        pthread_create(&writers[i], NULL, writerThreadWrapper, this);
    }
    std::cout << "Finshied creatin threads" << std::endl;
    // Wait for all threads to finish
    for (int i = 0; i < numReaders; i++) {
        pthread_join(readers[i], NULL);
    }
    std::cout << "Finshied read threads" << std::endl;
    for (int i = 0; i < numWriters; i++) {
        pthread_join(writers[i], NULL);
    }
    std::cout << "Finshied write threads" << std::endl;
}
