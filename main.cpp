#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include "generate.cpp"

bool parseInput(int argc, char* argv[], int& nx, int& ny, int& k1, int& k2) {
    if (argc == 5) {
        // Parse from command line arguments
        try {
            nx = std::stoi(argv[1]);
            ny = std::stoi(argv[2]);
            k1 = std::stoi(argv[3]);
            k2 = std::stoi(argv[4]);
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
        
        if (file >> nx >> ny >> k1 >> k2) {
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
        std::cerr << "  " << argv[0] << " nx ny k1 k2     (read from command line)\n";
        std::cerr << "  " << argv[0] << " filename        (read from file)\n";
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

int main(int argc, char* argv[]) {
    int nx, ny, k1, k2;
    
    if (!parseInput(argc, argv, nx, ny, k1, k2)) {
        return 1;
    }
    
    if (nx <= 0 || ny <= 0 || k1 < 0 || k2 < 0) {
        std::cerr << "Error: All values must be positive integers.\n";
        return 1;
    }

    setDebugFromEnv();
    
    auto graph = generate(nx, ny, k1, k2);
    
    return 0;
}