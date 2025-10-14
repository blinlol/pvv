#include <iostream>
#include <vector>
#include <iomanip>
#include <chrono>
#include <string>

bool DEBUG;

template<typename T>
void printVector2D(const std::vector<std::vector<T>>& vec) {
    if (vec.empty()) {
        std::cout << "Empty 2D vector\n";
        return;
    }
    
    for (const auto& row : vec) {
        for (const auto& element : row) {
            std::cout << element << "\t";
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

class Timer {
private:
    std::string name;
    std::chrono::high_resolution_clock::time_point start;
    
public:
    Timer(const std::string& funcName) : name(funcName) {
        start = std::chrono::high_resolution_clock::now();
    }
    
    ~Timer() {
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        std::cout << "⏱️  " << name << " executed in " << duration.count() / 1000000.0 << "s\n";
    }
};

// Macro for easy usage - automatically uses function name
#define MEASURE_FUNCTION Timer timer(__func__);
