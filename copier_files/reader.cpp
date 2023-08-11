/**
 * startup code provided by Paul Miller for COSC1114 - Operating Systems
 * Principles
 **/
//g++ main.cpp reader.cpp writer.cpp -o file_copier
#include "reader.h"
#include <fstream>
#include <functional>
#include <iostream>

reader::reader(const std::string& name, writer& mywriter) : thewriter(mywriter) {
    /* open the file for reading */
    in.open(name);
    std::cout << name << std::endl;
}
void reader::run() {
    /* read in each line of the file and append it to the writer's queue */
    std::string line;
    std::cout << "reader running" << std::endl;
    
    //read the file 
    while (std::getline(in, line)) {
        thewriter.append(line);
        std::cout<< line << std::endl;
    }

    /* close the file */
    in.close();
    std::cout << "reader finished" << std::endl;

}
