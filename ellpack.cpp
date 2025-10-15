#include <vector>
#include <algorithm>
#include <iostream>
#include <cmath>
#include "utils.h"

class Ellpack {
    private:
    int nx, ny, k1, k2;

    int imjmToM(int im, int jm) {
        return im * (nx+1) + jm;
    }

    void createJa(){
        MEASURE_FUNCTION

        std::vector<std::vector<int>> adjList(n);

        { // iter over nodes, connect with down and right neighbours
        int M, Q, R;
        for (int im=0; im < ny + 1; im++) {
            for (int jm=0; jm < nx + 1; jm++) {
                M = imjmToM(im, jm);
                Q = imjmToM(im+1, jm);
                R = imjmToM(im, jm + 1);

                // connect with down neighbour
                if (Q < n) {
                    adjList[M].push_back(Q);
                    adjList[Q].push_back(M);
                }

                // connect with right neighbour
                if (R < n && jm < nx ) { 
                    adjList[M].push_back(R);
                    adjList[R].push_back(M);
                }
            }
        }
        }
      
        { // iter over cells, connect with diag
        int it, jt, iq, jq, ir, jr, Q, R;
        for (int t=0; t < nx * ny; t++) {
            if (k1 <= t % (k1 + k2)) {
                
                it = t / nx;
                jt = t % nx;

                iq = it;
                jq = jt + 1;
                Q = imjmToM(iq, jq);

                ir = it+1;
                jr = jt;
                R = imjmToM(ir, jr);

                adjList[Q].push_back(R);
                adjList[R].push_back(Q);
            }
        }
        }

        // connect each node with itself
        for (int d=0; d < n; d++){
            adjList[d].push_back(d);
        }

        { // create ja from adjList
        int last = 0;
        int i;
        for (int M=0; M < n; M++) {
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

    Ellpack(int nx, int ny, int k1, int k2): nx(nx), ny(ny), k1(k1), k2(k2) {
        n = (nx+1) * (ny+1);

        ja = std::move(std::vector<int>(n * maxNeighbours));
        a = std::move(std::vector<double>(ja.size()));

        createJa();
    };

    void fill(){
        MEASURE_FUNCTION

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

Ellpack generate(int nx, int ny, int k1, int k2) {
    return Ellpack(nx, ny, k1, k2);
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