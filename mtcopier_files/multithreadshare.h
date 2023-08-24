#include <pthread.h>
#include <queue>
#include <deque>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <chrono>  


#ifndef MTSHARE
#define MTSHARE

class multithreadshare{

    public: 
        static std::ifstream in;
        static std::ofstream out;
        static int count;
        static std::deque<std::vector<char>> queue;
        static bool terminateWriters;
        multithreadshare(const std::string& inputFile, const std::string& outputFile, const int count);
        static void* readerThread(void* arg);
        static void* writerThread(void* arg);
        void run();
        static void terminateWriterThreads();
        static double readerLockTime;
        static double writerLockTime;
        static double readerWaitTime;
        static double writerWaitTime;
        static double readerCalled;
        static double writerCalled;
        static std::chrono::high_resolution_clock::time_point start; 

};

#endif