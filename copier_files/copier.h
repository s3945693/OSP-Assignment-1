#include <queue>
#include <deque>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>


#ifndef copy
#define copy

class copier{

    public: 
        std::ifstream in;
        std::ofstream out;
        int count;
        std::deque<std::vector<char>> queue;
        copier(const std::string& inputFile, const std::string& outputFile);
        void readerRun();
        void writerRun();
        void run();
};

#endif