#include <vector>
#include <iostream>
#include <omp.h>

#include "utils.h"
#include "ellpack.cpp"
#include "solver.h"


void test_axpy(int n, int iter) {
    double x = 1234.5678;
    std::vector<double> a(n);
    std::vector<double> b(n);
    for (int i = 0; i<n; i++) {
        a[i] = i * i * 0.3242 + 12.234;
        b[i] = i + (i - 213.434) * i;
    }
    std::vector<double> res(a.size());
    double start, end, sum = 0.0;
    {
    MEASURE_FUNCTION_NAME("axpy full time")
    start = omp_get_wtime();
    for (int it=0; it<iter; it++) {
        axpy(res, x, a, b);
        sum += res[it % res.size()];
    }
    end = omp_get_wtime();
    }
    std::cout << "axpy avg time " << (end - start) / iter << std::endl;
    std::cout << "axpy " << sum << std::endl;
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

    const int iter = 20;

    test_axpy(nx, iter);
}
