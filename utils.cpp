#include <iostream>
#include <vector>
#include <iomanip>

bool DEBUG;

template<typename T>
void printVector2D(const std::vector<std::vector<T>>& vec) {
    if (vec.empty()) {
        std::cout << "Empty 2D vector\n";
        return;
    }
    
    for (const auto& row : vec) {
        for (const auto& element : row) {
            std::cout << element << " ";
        }
        std::cout << "\n";
    }
    std::cout << std::endl;
}


template<typename T>
void printVector1D(const std::vector<T>& vec) {
    if (vec.empty()) {
        std::cout << "Empty 1D vector\n";
        return;
    }
    for (const auto& element : vec) {
        std::cout << element << " ";
    }
    std::cout << std::endl;
}
