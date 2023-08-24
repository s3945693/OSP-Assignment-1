/**
 * startup code provided by Paul Miller for COSC1114 - Operating Systems
 * Principles
 **/

//scl enable devtoolset-11 bash

#include <cstdlib>
#include <time.h>
#include "copier.h"
/* global variables if needed go here */
int main(int argc, char** argv) {
    /* check command line arguments */
    
    /* load the file and copy it to the destination */
    if (argc != 3) {
        std::cerr << "Usage: ./CHANGETHIS <source> <destination>\n";
        return EXIT_FAILURE;
    }

    copier m(argv[1], argv[2]);
    m.run();
    return EXIT_SUCCESS;
}
