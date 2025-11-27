#include <vector>
#include <fstream>

extern bool DEBUG;
extern int maxNeighbours;

#ifndef PRINT_H
#define PRINT_H

// template<typename T>
// void printVector2D(const std::vector<std::vector<T>>& vec);

// template<typename T>
// void printVector1D(const std::vector<T>& vec);


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
    
#endif

#ifndef TIMER_H
#define TIMER_H

class Timer {
private:
    std::string name;
    double start;

public:
    Timer(const std::string& funcName);
    
    ~Timer();
};

#endif

// Macro for easy usage - automatically uses function name
#define MEASURE_FUNCTION Timer timer(__func__);
#define MEASURE_FUNCTION_NAME(name) Timer timer(name);
