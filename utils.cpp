#include <iostream>
#include <vector>
#include <iomanip>
#include <chrono>
#include <string>
#include <omp.h>

#include "utils.h"

bool DEBUG;
int maxNeighbours=7;

// template<typename T>
// void printVector2D(const std::vector<std::vector<T>>& vec) {
//     if (vec.empty()) {
//         std::cout << "Empty 2D vector\n";
//         return;
//     }
    
//     for (const auto& row : vec) {
//         for (const auto& element : row) {
//             std::cout << element << "\t";
//         }
//         std::cout << "\n";
//     }
//     std::cout << std::endl;
// }


// template<typename T>
// void printVector1D(const std::vector<T>& vec) {
//     if (vec.empty()) {
//         std::cout << "Empty 1D vector\n";
//         return;
//     }
//     for (const auto& element : vec) {
//         std::cout << element << " ";
//     }
//     std::cout << std::endl;
// }
    
Timer::Timer(const std::string& funcName) : name(funcName) {
    start = omp_get_wtime();
}
    
Timer::~Timer() {
    auto end = omp_get_wtime();
    auto duration = end - start;
    std::cout << "⏱️  " << name << " executed in " << duration << "s\n";
}
