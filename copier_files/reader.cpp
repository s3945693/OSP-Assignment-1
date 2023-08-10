/**
 * startup code provided by Paul Miller for COSC1114 - Operating Systems
 * Principles
 **/
#include "reader.h"
#include <functional>
#include <iostream>
reader::reader(const std::string& name, writer& mywriter) : thewriter(mywriter) {
    /* open the file for reading */
    in.open(name);
}
void reader::run() {
    /* read in each line of the file and append it to the writer's queue */
    std::string line;
    while (std::getline(in, line)) {
        thewriter.append(line);
    }
    /* close the file */
    in.close();

}
