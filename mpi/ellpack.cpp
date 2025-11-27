#include <vector>
#include <algorithm>
#include <iostream>
#include <cmath>
#include "utils.h"

class Ellpack {
    private:
    int nx, ny, k1, k2;
    ProcessInfo info;

    int imjmToM(int im, int jm) {
        return im * (nx+1) + jm;
    }

    void createJa(){
        std::vector<std::vector<int>> adjList(n);

        #pragma omp parallel 
        { // start parallel
        
        // iter over nodes
        #pragma omp for
        for (int im=0; im < ny + 1; im++) {
            for (int jm=0; jm < nx + 1; jm++) {
                int M = imjmToM(im, jm);

                int Down = imjmToM(im+1, jm);
                int Right = imjmToM(im, jm + 1);
                int Left = imjmToM(im, jm - 1);
                int Up = imjmToM(im - 1, jm);

                if (Down < n) {
                    adjList[M].push_back(Down);
                }

                if (Right < n && jm < nx ) { 
                    adjList[M].push_back(Right);
                }

                if (Left >= 0 && jm > 0) {
                    adjList[M].push_back(Left);
                }

                if (Up >= 0) {
                    adjList[M].push_back(Up);
                }
            }
        }

        #pragma omp master
        { // iter over cells, connect with diag
        for (int t=0; t < nx * ny; t++) {
            if (k1 <= t % (k1 + k2)) {
                
                int it = t / nx;
                int jt = t % nx;

                int iq = it;
                int jq = jt + 1;
                int Q = imjmToM(iq, jq);

                int ir = it+1;
                int jr = jt;
                int R = imjmToM(ir, jr);

                adjList[Q].push_back(R);
                adjList[R].push_back(Q);
            }
        }
        }

        
        // connect each node with itself
        #pragma omp master
        for (int d=0; d < n; d++){
            adjList[d].push_back(d); ///
        }

        #pragma omp master
        { // create ja from adjList
        int last = 0;
        for (int M=0; M < n; M++) {
            int i;
            std::sort(adjList[M].begin(), adjList[M].end());
            for (i = 0; i < adjList[M].size(); i++) {
                ja[last] = adjList[M][i];
                last++;
            }
            for (; last < (M + 1) * maxNeighbours; last ++) {
                ja[last] = adjList[M][i-1];
            }
        }
        }

        } // end parallel

        if (DEBUG) {
            std::cout << "adjList:\n";
            printVector2D(adjList);
            std::cout << "\nja:\n";
            printVector1D(ja);
        }
    }

    public:
    // num nodes in graph
    int n;

    std::vector<int> ja;
    std::vector<double> a;

    Ellpack(int nx, int ny, int k1, int k2, ProcessInfo info): nx(nx), ny(ny), k1(k1), k2(k2), info(info) {
        MEASURE_FUNCTION

        n = (nx+1) * (ny+1);

        ja = std::move(std::vector<int>(n * maxNeighbours));
        a = std::move(std::vector<double>(ja.size()));

        createJa();
    };

    void fill(){
        MEASURE_FUNCTION

        #pragma omp parallel for
        for (int i=0; i < n; i++) {
            double nonDiagSum = 0;
            int diagj = -1;
            for (int jInd=i*maxNeighbours; jInd < (i + 1) * maxNeighbours; jInd++) {
                int j = ja[jInd];

                // check cols end (may be it's not neccessary)
                if (jInd != i*maxNeighbours && j == ja[jInd - 1]) {
                    break;
                }

                if (j == i) {
                    diagj = jInd;
                    continue;
                }

                a[jInd] = std::cos(double(i * j + i + j));
                nonDiagSum += std::abs(a[jInd]);
            }

            a[diagj] = 1.234 * nonDiagSum;
        }

        if (DEBUG) {
            std::cout << "\nfilled a:\n";
            printVector1D(a);
        }
    }
};

Ellpack generate(int nx, int ny, int k1, int k2, ProcessInfo info) {
    return Ellpack(nx, ny, k1, k2, info);
}

// return pair <A, b>
// A is a portrait of adjacency matrix
// b is right side of SLA
std::pair<std::vector<double>, std::vector<double>> fill(Ellpack& graph) {
    graph.fill();

    std::vector<double> b(graph.n);
    for (int i=0; i < b.size(); i++) {
        b[i] = std::sin(i);
    }

    if (DEBUG) {
        std::cout << "\nb:\n";
        printVector1D(b);
    }

    return std::make_pair(graph.a, b);
}