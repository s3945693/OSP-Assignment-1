#include <pthread.h>
#include <queue>
#include <deque>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>


#ifndef MTSHARE
#define MTSHARE

class multithreadshare{

    public: 
        std::ifstream in;
        std::ofstream out;
        int count;
        static std::queue<std::string> queue;
        bool terminateWriters = false;
        multithreadshare(const std::string& inputFile, const std::string& outputFile, const int count);

        void* readerThread(void* arg);
        void* writerThread(void* arg);
        void run();
        void terminateWriterThreads();
};

#endif