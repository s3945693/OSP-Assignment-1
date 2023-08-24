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
#include <unistd.h>
#include <filesystem>

// scl enable devtoolset-11 bash
// s3945693@titan.csit.rmit.edu.au
// g++ -Wall -Werror -std=c++20 -lpthread multithreadshare.cpp main.cpp -o mtCopier -t
// ./mtcopier 10 cop.txt out.txt
// ./mtcopier 100 ~e70949/shared/osp2023/data.512m /tmp/s3945693output
// valgrind --track-origins=yes --leak-check=full --show-leak-kinds=all ./mtcopier 50 data.512m test.512m
bool isNumber(std::string s);
int main(int argc, char** argv) {
    /**
     * check command line arguments
     **/
    
    if (argc != 4) {
        std::cerr << "Usage: ./mtcopier <thread count> <source> <destination>\n";
        
        return EXIT_FAILURE;
    }
    if (argv[2] == argv[3]){
        std::cerr << "Source and destination files must be different\n";
        return EXIT_FAILURE;
    }

    if (!isNumber(argv[1])){ 
        std::cout << "The third argument was not a number" << std::endl;
        return EXIT_FAILURE;  
    }
    if (std::stoi(argv[1])<1){
        std::cerr << "Thread count must be greater than 0\n";
        return EXIT_FAILURE;
    }
    if(!std::filesystem::exists(argv[2])){
        std::cout << "Read file does not exists.\n";
        return EXIT_FAILURE;
    }

    int count = std::stoi(argv[1]);
    multithreadshare* test = new multithreadshare(argv[2],argv[3], count);
    test->run();
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