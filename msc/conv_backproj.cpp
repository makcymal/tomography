#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>

using namespace std;



int main() {
    auto config = Config();
    Matrix grid(2 * config.pts + 1, vector<double>(config.dirs)),
            conv(2 * config.pts + 1, vector<double>(config.dirs));

    ifstream grid_buff;
    grid_buff.open(GRID_FILE);

    if (grid_buff.is_open()) {
        for (auto pt = 0; pt < 2 * config.pts + 1; ++pt) {
            for (auto dir = 0; dir < config.dirs; ++dir) {
                grid_buff >> grid[pt][dir];
            }
        }
    }
    grid_buff.close();

    convolution(grid, conv, config);

    double backproj = 0, max = 0;
    ofstream backproj_buff;
    backproj_buff.open(BACKPROJ_FILE);

    for (auto i = -config.nx; i <= config.nx; ++i) {
        double y = -(double) i / config.nx;
        for (auto j = -config.ny; j <= config.ny; ++j) {
            double x = (double) j / config.ny;

            if (x * x + y * y >= 1) backproj = 0;
            else backprojection(backproj, x, y, conv, config);

            if (backproj <= 0) backproj = 0;
            else if (max < backproj) max = backproj;

            backproj_buff << backproj << ' ';
        }
        backproj_buff << '\n';
    }
    backproj_buff << max;
    backproj_buff.close();
}
