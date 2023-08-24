/**
 * startup code provided by Paul Miller for COSC1114 - Operating Systems
 * Principles
 **/

//scl enable devtoolset-11 bash
#include <filesystem>

#include <cstdlib>
#include <time.h>
#include "copier.h"
/* global variables if needed go here */
int main(int argc, char** argv) {
    /* check command line arguments */
    
    /* load the file and copy it to the destination */
    if (argc != 3) {
        std::cerr << "Usage: ./copier <source> <destination>\n";
        return EXIT_FAILURE;
    }
    
    if(!std::filesystem::exists(argv[1])){
        std::cout << "Read file does not exists.\n";
        return EXIT_FAILURE;
    }
    if (argv[1] == argv[2]){
        std::cerr << "Source and destination files must be different\n";
        return EXIT_FAILURE;
    }
    copier m(argv[1], argv[2]);
    m.run();
    return EXIT_SUCCESS;
}
