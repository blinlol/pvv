#include <vector>
#include <iostream>

#ifndef SOLVER
#define SOLVER

struct Solution {
    public:
    // solution vector
    std::vector<double> x;
    // number of iterations
    int n;
    // l2 norm of res
    double res;

    Solution(std::vector<double> x, int n, double res);
};

std::ostream& operator<<(std::ostream& os, const Solution& sol);

// y = A * x
void spmv(std::vector<double>& res, std::vector<int>& JA, std::vector<double>& A, std::vector<double>& x);

// (a, b)
double dot(std::vector<double>& a, std::vector<double>& b);

// a*x+y
voud axpy(std::vector<double> res, double a, std::vector<double>& x, std::vector<double>& y);

#endif
