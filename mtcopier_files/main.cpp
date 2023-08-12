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



int main(int argc, char** argv) {
    /**
     * check command line arguments
     **/
    //./mtCopier mtest.txt mtestR.txt
    if (argc != 3) {
        std::cerr << "Usage: ./CHANGETHIS <source> <destination> <thread count>\n";
        return EXIT_FAILURE;
    }
    multithreadshare test(argv[1],argv[2]);
    test.run();


    return EXIT_SUCCESS;
}
