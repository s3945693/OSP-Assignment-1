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

//g++ -Wall -Werror -pthread multithreadshare.cpp main.cpp -o mtCopier
//  ./mtCopier mtest.txt mtestR.txt
bool isNumber(std::string s);
int main(int argc, char** argv) {
    /**
     * check command line arguments
     **/
    
    if (argc != 4) {
        std::cerr << "Usage: ./CHANGETHIS <source> <destination> <thread count>\n";
        
        return EXIT_FAILURE;
    }
    if (isNumber(argv[3])){ 
            if (std::stoi(argv[3])<1){
                std::cerr << "Thread count must be greater than 0\n";
            }
        return EXIT_FAILURE;  
    }
    
    int count = std::stoi(argv[3]);
    multithreadshare test(argv[1],argv[2], count);
    test.run();


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