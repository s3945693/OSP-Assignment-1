#include <pthread.h>
#include <queue>
#include <deque>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

//g++ -Wall -Werror -pthread multithreadshare.cpp main.cpp -o mtCopier
#ifndef MTSHARE
#define MTSHARE

class multithreadshare{

    public: 
        std::ifstream in;
        std::ofstream out;
        int count;
        static std::queue<std::string> queue;
        multithreadshare(const std::string& inputFile, const std::string& outputFile);

        void* readerThread(void* arg);
        void* writerThread(void* arg);
        void run();
};

#endif