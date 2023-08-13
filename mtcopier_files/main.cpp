/**
 * startup code provided by Paul Miller for COSC1114 - Operating Systems
 * Principles
 **/
#include <pthread.h>
#include <queue>
#include <deque>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "multithreadshare.h"
#include <time.h>
// scl enable devtoolset-11 bash
// s3945693@titan.csit.rmit.edu.au
// g++ -Wall -Werror -std=c++20 -lpthread multithreadshare.cpp main.cpp -o mtCopier -t
// ./mtcopier cop.txt out.txt 10
// ./mtcopier ~e70949/shared/osp2023/data.512m /tmp/s3945693output 100
bool isNumber(std::string s);
int main(int argc, char** argv) {
    /**
     * check command line arguments
     **/
    
    if (argc != 4) {
        std::cerr << "Usage: ./CHANGETHIS <source> <destination> <thread count>\n";
        
        return EXIT_FAILURE;
    }
    if (argv[1] == argv[2]){
        std::cerr << "Source and destination files must be different\n";
        return EXIT_FAILURE;
    }
    if (!isNumber(argv[3])){ 
            if (std::stoi(argv[3])<1){
                std::cerr << "Thread count must be greater than 0\n";
            }
        std::cout << "The third argument was not a number" << std::endl;
        return EXIT_FAILURE;  
    }
    
    int count = std::stoi(argv[3]);
    multithreadshare* test = new multithreadshare(argv[1],argv[2], count);
    clock_t start, end;
    double cpu_time_used;
    start = clock();
    
    test->run();
    end = clock();

    cpu_time_used = (double)(end - start);

    // Convert CPU time to seconds
    double seconds = cpu_time_used / CLOCKS_PER_SEC;

    printf("CPU time used: %f seconds\n", seconds);
    delete test;

    return EXIT_SUCCESS;
}

bool isNumber(std::string s){
    std::string::const_iterator it = s.begin();
    char dot = '.';
    int nb_dots = 0;
    while (it != s.end()) 
    {
        if (*it == dot)
        {
            nb_dots++;
            if (nb_dots>1)
            {
                break;
            }
        }   
        else if (!isdigit(*it))
        {
            break;
        } 

        ++it;
    }
    return !s.empty() && s[0] != dot && it == s.end();
}