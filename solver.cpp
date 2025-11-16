#include <vector>
#include <cmath>
#include <iostream>
#include "utils.h"

#define SOLVER

struct Solution {
    public:
    // solution vector
    std::vector<double> x;
    // number of iterations
    int n;
    // l2 norm of res
    double res;

    Solution(std::vector<double> x, int n, double res): x(x), n(n), res(res){};
};

std::ostream& operator<<(std::ostream& os, const Solution& sol) {
    os << "Solution\n\tx: ";
    printVector1D(sol.x);

    os << "\tn: " << sol.n  
       << "\n\tres: " << sol.res << std::endl;
    return os;
}

// y = A * x
void spmv(std::vector<double>& y, std::vector<int>& JA, std::vector<double>& A, std::vector<double>& x) {
    MEASURE_FUNCTION

    int N = JA.size() / maxNeighbours;

    if (N != y.size()) {
        throw "wrong y length";
    }

    #pragma omp parallel for
    for(int i=0; i<N; ++i){
        double sum = 0.0;
        {
        for(int _j = i * maxNeighbours; _j<(i+1) * maxNeighbours; ++_j){
            if (_j != i * maxNeighbours && JA[_j] == JA[_j - 1]) {
                break;
            }
            sum += A[_j] * x[JA[_j]];
        }
        }
        y[i] = sum;
    }
}

// y = D * x, where D is diagonal matrix
void spmvDiag(std::vector<double>& y, std::vector<double>& diag, std::vector<double>& x) {
    MEASURE_FUNCTION

    if (y.size() != x.size()) {
        throw "y.size() != x.size()";
    }

    #pragma omp parallel for
    for (int i=0; i<x.size(); i++) {
        y[i] = diag[i] * x[i];
    }
}

// D^-1 as vector
std::vector<double> reverseDiag(std::vector<double>& D) {
    std::vector<double> revD(D.size());
    for (int i=0; i < D.size(); i++) {
        revD[i] = 1.0 / D[i];
    }

    return revD;
}

// (a, b)
double dot(std::vector<double>& a, std::vector<double>& b) {
    MEASURE_FUNCTION

    double res=0;
    const int len = a.size();

    #pragma omp parallel for reduction(+:res)
    for (int i=0; i<len; i++) {
        res += a[i] * b[i];
    }

    return res;
}

// a*x+y
void axpy(std::vector<double>& res, double a, std::vector<double>& x, std::vector<double>& y) {
    MEASURE_FUNCTION

    if (res.size() != x.size()) {
        throw "res.size() != x.size()";
    }

    #pragma omp parallel for
    for (int i=0; i<x.size(); i++) {
        res[i] = a * x[i] + y[i];
    }
}

// || x ||
double L2(std::vector<double>& x) {
    return std::sqrt(dot(x, x));
}

// res = |Ax - b|
double calcRes(std::vector<int>& JA, std::vector<double>& A, std::vector<double>& x, std::vector<double>& b){
    MEASURE_FUNCTION

    int N = JA.size() / maxNeighbours;

    static std::vector<double> Ax(N);
    spmv(Ax, JA, A, x);
    static std::vector<double> res(x.size());

    #pragma omp parallel for
    for (int i=0; i<x.size(); i++) {
        res[i] = Ax[i] - b[i];
    }
    return L2(res);
}


Solution solve(int n, std::vector<int> JA, std::vector<double> A, std::vector<double> b, double eps, int maxit) {
    MEASURE_FUNCTION

    std::vector<double> x0(n);

    std::vector<double> r_0=b;
    
    std::vector<double> M(n);
    for (int i=0; i < n; i++) {
        for (int _j=i*maxNeighbours; _j < (i+1) * maxNeighbours; _j++) {
            if (JA[_j] == i) {
                M[i] = A[_j];
                break;
            }
        }
    }
    auto reverseM = reverseDiag(M);


    double ro_prev, ro_new;

    int N = JA.size() / maxNeighbours;
    
    std::vector<double> q(N), z(N);
    std::vector<double> p_new(N), p_prev(N), tmp;
    std::vector<double> x_new(N), r_new(N), x_prev(N), r_prev(N);

    int k = 0;
    r_prev = r_0;
    x_prev = std::vector<double>(b.size());

    do {
        {
        MEASURE_FUNCTION_NAME("solve_loop")
        
        k++;
        spmvDiag(z, reverseM, r_prev);
        ro_new = dot(r_prev, z);
        if (k == 1) {
            p_new.swap(z);
        } else {
            double beta = ro_new / ro_prev;
            axpy(p_new, beta, p_prev, z);
        }


        spmv(q, JA, A, p_new);

        double alpha = ro_new / dot(p_new, q);
        axpy(x_new, alpha, p_new, x_prev);
        axpy(r_new, -alpha, q, r_prev);

        auto res = calcRes(JA, A, x_new, b);

        std::cout << "#" << k << ": " << res << std::endl;

        p_prev.swap(p_new);
        x_prev.swap(x_new);
        r_prev.swap(r_new);

        ro_prev = ro_new;
        }
    } while (ro_new > eps * eps && k < maxit);

    return Solution(x_new, k, calcRes(JA, A, x_new, b));
}
