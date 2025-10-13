#include <vector>
#include <algorithm>
#include <iostream>
#include "utils.cpp"

const int maxNeighbours = 7;

class Ellpack {
    private:
    int nx, ny, k1, k2;

    int imjmToM(int im, int jm) {
        return im * (nx+1) + jm;
    }

    public:
    // num nodes in graph
    int n;

    std::vector<int> ja;
    std::vector<std::vector<int>> a;

    Ellpack(int nx, int ny, int k1, int k2): nx(nx), ny(ny), k1(k1), k2(k2) {
        n = (nx+1) * (ny+1);

        ja = std::move(std::vector<int>(n * maxNeighbours));

        std::vector<std::vector<int>> adjList(n);

        {
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
      
        {
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

        for (int d=0; d < n; d++){
            adjList[d].push_back(d);
        }

        {
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
    };
};

Ellpack generate(int nx, int ny, int k1, int k2) {
    return Ellpack(nx, ny, k1, k2);
}