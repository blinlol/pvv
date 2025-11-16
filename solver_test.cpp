#include <vector>
#include <iostream>
#include <omp.h>

#include "utils.h"
#include "ellpack.cpp"
#include "solver.h"


void test_dot() {
    int n = 20000000;
    std::vector<double> a(n);
    std::vector<double> b(n);
    for (int i = 0; i<n; i++) {
        a[i] = i * i * 0.3242 + 12.234;
        b[i] = i + (i - 213.434) * i;
    }
    std::cout << "dot " << dot(a, b) << std::endl;
}

void test_spmv() {
    int nx=5000, ny=5000, k1=7, k2=13;
    
    auto graph = generate(nx, ny, k1, k2);
    graph.fill();

    std::vector<double> x(graph.n);
    for (int i = 0; i<x.size(); i++) {
        x[i] = i * i * 0.3242 + 12.234;
    }

    std::vector<double> res(graph.ja.size() / maxNeighbours);
    spmv(res, graph.ja, graph.a, x);
    std::cout << "spmv ";
    printVector1D(std::vector<double>(res.begin() + 1000, res.begin() + 1000 + 5));
}

void test_axpy() {
    int n = 20000000;
    double x = 1234.5678;
    std::vector<double> a(n);
    std::vector<double> b(n);
    for (int i = 0; i<n; i++) {
        a[i] = i * i * 0.3242 + 12.234;
        b[i] = i + (i - 213.434) * i;
    }
    auto res = axpy(x, a, b);
    std::cout << "axpy ";
    printVector1D(std::vector<double>(res.begin()+1000, res.begin() + 1000 + 5));
}

bool parseInput(int argc, char* argv[], int& nx, int& ny, int& k1, int& k2, int& t) {
    if (argc == 6) {
        // Parse from command line arguments
        try {
            nx = std::stoi(argv[1]);
            ny = std::stoi(argv[2]);
            k1 = std::stoi(argv[3]);
            k2 = std::stoi(argv[4]);
            t = std::stoi(argv[5]);
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Error: Invalid command line arguments. Please provide integers only.\n";
            return false;
        }
    }
    else if (argc == 2) {
        // Parse from file
        std::ifstream file(argv[1]);
        if (!file.is_open()) {
            std::cerr << "Error: Cannot open file '" << argv[1] << "'\n";
            return false;
        }
        
        if (file >> nx >> ny >> k1 >> k2 >> t) {
            file.close();
            return true;
        } else {
            std::cerr << "Error: Invalid file format. Expected 4 integers.\n";
            file.close();
            return false;
        }
    }
    else {
        std::cerr << "Usage:\n";
        std::cerr << "  " << argv[0] << " nx ny k1 k2 t   (read from command line)\n";
        std::cerr << "  " << argv[0] << " filename        (read from file)\n";
        return false;
    }
}

int main(int argc, char *argv[]) {
    int nx, ny, k1, k2, t;
    
    if (!parseInput(argc, argv, nx, ny, k1, k2, t)) {
        return 1;
    }
    
    omp_set_num_threads(t);
 
    // test_dot();
    test_spmv();
    test_axpy();
}
