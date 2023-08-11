/**
 * startup code provided by Paul Miller for COSC1114 - Operating Systems
 * Principles
 **/
#include "writer.h"

#include "reader.h"

/**
 * implement these functions requred for the writer class
 **/
void writer::init(const std::string& name) {
    out.open(name);
}

void writer::run() {}

void* writer::runner(void* arg) { return nullptr; }

void writer::append(const std::string& line) {
    queue.push_back(line);
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

