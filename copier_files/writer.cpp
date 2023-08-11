/**
 * startup code provided by Paul Miller for COSC1114 - Operating Systems
 * Principles
 **/

#include "writer.h"

/**
 * provide your implementation for the writer functions here
 **/
writer::writer(const std::string& name):out(name) {
    /* open the file for writing */
    
}

void writer::run() {

    //check if there are items in the queue
    if (queue.empty()) {
        std::cout << "Queue is empty" << std::endl;
        return;
    }

    /* write each line of the queue to the file */
    while (!queue.empty()) {
        out<< queue.front() << std::endl;
        queue.pop_front();
    }
    std::cout << "writer finished" << std::endl;
    /* close the file */
    out.close();
}

void writer::append(const std::string& line) {
    /* open the file for writing */
    queue.push_back(line);
}
