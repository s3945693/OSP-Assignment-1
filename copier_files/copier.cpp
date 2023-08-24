#include "copier.h"
#include <chrono>
#include <iostream>

copier::copier(const std::string& inputFile, const std::string& outputFile) {
    in.open(inputFile);
    out.open(outputFile);
}

void copier::readerRun() {
    char buffer[50000];
    while (in.read(buffer, sizeof(buffer))) {
        queue.push_back(std::vector<char>(buffer, buffer + sizeof(buffer)));
    }
    //last lines are pushed to queue
    queue.push_back(std::vector<char>(buffer, buffer + in.gcount()));
}

void copier::writerRun() {
    while (!queue.empty()) {
        auto data = queue.front();
        out.write(data.data(), data.size());
        queue.pop_front();
    }
}

void copier::run() {
    auto start = std::chrono::high_resolution_clock::now();
    readerRun();
    auto end = std::chrono::high_resolution_clock::now();
    double total = std::chrono::duration<double>(end - start).count();
    std::cout << "Total Read time: " << total << std::endl;

    start = std::chrono::high_resolution_clock::now();
    writerRun();
    end = std::chrono::high_resolution_clock::now();
    total = std::chrono::duration<double>(end - start).count();
    std::cout << "Total Write time: " << total << std::endl;
}
