#include <cmath>
#include <vector>

#include "algo.h"
#include "glob.h"


void convolution(Matrix &grid, Matrix &conv, Config &config) {
    double sum;

    for (auto pt = 0; pt < 2 * config.pts + 1; ++pt) {
        for (auto dir = 0; dir < config.dirs; ++dir) {
            sum = 0;
            for (auto pti = 0; pti < 2 * config.pts + 1; ++pti) {
                sum += grid[pti][dir] / (1 - 4 * (pt - pti) * (pt - pti));
            }
            conv[pt][dir] = sum;
        }
    }
}

void backprojection(double &backproj, double &x, double &y, Matrix &conv, Config &config) {
    double sum = 0;

    for (auto dir = 0; dir < config.dirs; ++dir) {
        double phi = PI * (dir - 1) / config.dirs;
        double s = x * cos(phi) + y * sin(phi);
        double rs = s * config.pts;
        int is = floor(s * config.pts);
        double s_mantissa = abs(rs - is);

        sum += (1 - s_mantissa) * conv[config.pts + is][dir] +
               s_mantissa * conv[config.pts + is + 1][dir];
    }

    backproj = 2 * config.pts * sum / (PI * config.dirs);
}

