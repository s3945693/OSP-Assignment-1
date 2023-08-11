/**
 * startup code provided by Paul Miller for COSC1114 - Operating Systems
 * Principles
 **/
#include "reader.h"
#include "writer.h"
#include <cstdlib>
/* global variables if needed go here */
int main(int argc, char** argv) {
    /* check command line arguments */
    
    /* load the file and copy it to the destination */
    if (argc != 3) {
        std::cerr << "Usage: ./CHANGETHIS <source> <destination>\n";
        return EXIT_FAILURE;
    }

    /* create a reader object and a writer object */
    writer w(argv[2]);
    reader r(argv[1], w);
    
    r.run();
    w.run();
    return EXIT_SUCCESS;
}
