#include <vector>
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

        // adjacenty matrix
        std::vector<std::vector<bool>> adj(n, std::vector<bool>(n));
        {
        int M, Q, R;

        for (int im=0; im < ny + 1; im++) {
            for (int jm=0; jm < nx + 1; jm++) {
                M = imjmToM(im, jm);
                Q = imjmToM(im+1, jm);
                R = imjmToM(im, jm + 1);

                // connect with down neighbour
                if (Q < n) {
                    adj[M][Q] = true;
                    adj[Q][M] = true;
                }

                // connect with right neighbour
                if (R < n && jm < nx ) { 
                    adj[M][R] = true;
                    adj[R][M] = true;                    
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
                jq - jt + 1;
                Q = imjmToM(iq, jq);

                ir = it+1;
                jr = jt;
                R = imjmToM(ir, jr);

                adj[Q][R] = true;
                adj[R][Q] = true;
            }
        }
        }

        for (int d=0; d < n; d++){
            adj[d][d] = true;
        }



        if (DEBUG) {
            printVector2D(adj);
        }
    };
};

Ellpack generate(int nx, int ny, int k1, int k2) {
    return Ellpack(nx, ny, k1, k2);
}