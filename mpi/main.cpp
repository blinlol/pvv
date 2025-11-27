#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <climits>
#include <omp.h>
#include <mpi.h>

#include "utils.h"
#include "ellpack.cpp"
#include "solver.cpp"
#include "process.h"

bool parseInput(int argc, char* argv[], int& nx, int& ny, int& k1, int& k2, int& t, int& px, int& py) {
    if (argc == 8) {
        // Parse from command line arguments
        try {
            nx = std::stoi(argv[1]);
            ny = std::stoi(argv[2]);
            k1 = std::stoi(argv[3]);
            k2 = std::stoi(argv[4]);
            t = std::stoi(argv[5]);
            px = std::stoi(argv[6]);
            py = std::stoi(argv[7]);
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
        
        if (file >> nx >> ny >> k1 >> k2 >> t >> px >> py) {
            file.close();
            return true;
        } else {
            std::cerr << "Error: Invalid file format. Expected 7 integers.\n";
            file.close();
            return false;
        }
    }
    else {
        std::cerr << "Usage:\n";
        std::cerr << "  " << argv[0] << " nx ny k1 k2 t px py   (read from command line)\n";
        std::cerr << "  " << argv[0] << " filename              (read from file)\n";
        return false;
    }
}

void setDebugFromEnv() {
    const char* debug_env = std::getenv("DEBUG");
    if (debug_env != nullptr) {
        std::string debug_str = debug_env;
        // Check for various "true" values
        if (!debug_str.empty()) {
            DEBUG = true;
            std::cout << "Debug mode enabled via environment variable\n";
        }
    }
}

double getEpsFromEnv() {
    const char* envValue = std::getenv("EPS");
    double defaultEps = 0.01;
    
    if (envValue == nullptr) {
        return defaultEps;
    }
    
    try {
        // Convert string to double
        char* endPtr;
        double eps = std::strtod(envValue, &endPtr);
        
        // Check if conversion was successful
        if (endPtr == envValue || *endPtr != '\0') {
            throw std::invalid_argument("Invalid numeric format");
        }
        
        // Check for range errors
        if (errno == ERANGE) {
            throw std::out_of_range("Value out of range");
        }
        
        std::cout << "Loaded EPS from environment: " << eps << std::endl;
        return eps;
    }
    catch (const std::exception& e) {
        std::cerr << "Error parsing " << "EPS" << "='" << envValue 
                  << "': " << e.what() << std::endl;
        throw e;
    }
}

int getMaxItFromEnv() {
    const char* envValue = std::getenv("MAXIT");
    int defaultMaxit = 100;

    if (envValue == nullptr) {
        return defaultMaxit;
    }
    
    try {
        // Convert string to integer
        char* endPtr;
        errno = 0; // Reset errno before conversion
        long longValue = std::strtol(envValue, &endPtr, 10);
        
        // Check if conversion was successful
        if (endPtr == envValue) {
            throw std::invalid_argument("No digits found");
        }
        
        // Check if there are trailing characters
        if (*endPtr != '\0') {
            throw std::invalid_argument("Invalid characters in value");
        }
        
        // Check for range errors
        if (errno == ERANGE || longValue < INT_MIN || longValue > INT_MAX) {
            throw std::out_of_range("Value out of integer range");
        }
        
        int maxit = static_cast<int>(longValue);
        
        // Validate it's positive
        if (maxit <= 0) {
            throw std::invalid_argument("MAXIT must be positive");
        }
        
        std::cout << "Loaded MAXIT from environment: " << maxit << std::endl;
        return maxit;
    }
    catch (const std::exception& e) {
        std::cerr << "Error parsing " << "MAXIT" << "='" << envValue 
                  << "': " << e.what() << std::endl;
        throw e;
    }
}

int main(int argc, char* argv[]) {
    // Initialize MPI
    int mpi_result = MPI_Init(&argc, &argv);
    if (mpi_result != MPI_SUCCESS) {
        std::cerr << "Error: MPI_Init failed with code " << mpi_result << "\n";
        return 1;
    }
    
    int rank, size;
    mpi_result = MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (mpi_result != MPI_SUCCESS) {
        std::cerr << "Error: MPI_Comm_rank failed with code " << mpi_result << "\n";
        MPI_Abort(MPI_COMM_WORLD, mpi_result);
        return 1;
    }
    
    mpi_result = MPI_Comm_size(MPI_COMM_WORLD, &size);
    if (mpi_result != MPI_SUCCESS) {
        std::cerr << "Error: MPI_Comm_size failed with code " << mpi_result << "\n";
        MPI_Abort(MPI_COMM_WORLD, mpi_result);
        return 1;
    }
    
    int nx, ny, k1, k2, t, px, py;
    
    if (!parseInput(argc, argv, nx, ny, k1, k2, t, px, py)) {
        MPI_Finalize();
        return 1;
    }
    
    if (nx <= 0 || ny <= 0 || k1 < 0 || k2 < 0 || t < 0 || px <= 0 || py <= 0) {
        if (rank == 0) {
            std::cerr << "Error: nx, ny, t, px, py must be positive; k1, k2 must be non-negative.\n";
        }
        MPI_Finalize();
        return 1;
    }
    
    // Check that number of processes equals px * py
    if (size != px * py) {
        if (rank == 0) {
            std::cerr << "Error: Number of MPI processes (" << size 
                      << ") must equal px * py (" << px << " * " << py << " = " << px * py << ")\n";
        }
        MPI_Finalize();
        return 1;
    }
    
    omp_set_num_threads(t);

    setDebugFromEnv();

    double eps = getEpsFromEnv();
    int maxit = getMaxItFromEnv();

    ProcessInfo info{rank, size, px, py};
    
    auto graph = generate(nx, ny, k1, k2, info);

    auto ab = fill(graph);
    
    auto solution = solve(graph.n, graph.ja, ab.first, ab.second, eps, maxit);

    if (DEBUG) {
        std::cout << solution;
    }
    
    MPI_Finalize();
    return 0;
}