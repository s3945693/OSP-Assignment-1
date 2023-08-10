/**
 * startup code provided by Paul Miller for COSC1114 - Operating Systems
 * Principles
 **/
#include "reader.h"
#include <fstream>
#include <functional>
#include <iostream>
reader::reader(const std::string& name, writer& mywriter) : thewriter(mywriter) {
    /* open the file for reading */
    std::ifstream infile = std::ifstream(name);
    in = std::move(infile);
    std::cout << name << std::endl;
}
void reader::run() {
    /* read in each line of the file and append it to the writer's queue */
    std::string line;
    std::cout << "reader running" << std::endl;
    
    //read the file 
    while (std::getline(in, line)) {
        thewriter.append(line);
    }

    /* close the file */
    in.close();
    std::cout << "reader finished" << std::endl;

}
