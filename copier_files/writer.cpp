/**
 * startup code provided by Paul Miller for COSC1114 - Operating Systems
 * Principles
 **/

#include "writer.h"

/**
 * provide your implementation for the writer functions here
 **/
writer::writer(const std::string& name) {
    /* open the file for writing */
    out = std::ofstream(name);
}

void writer::run() {
    /* open the file for writing */
    out.open("output.txt");
    /* write each line of the queue to the file */
    for (auto line : queue) {
        out << line << std::endl;
    }
    /* close the file */
    out.close();
}

void writer::append(const std::string& line) {
    /* open the file for writing */
    queue.push_back(line);
}
