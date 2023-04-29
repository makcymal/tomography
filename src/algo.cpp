#include <cmath>
#include <vector>

#include "algo.h"
#include "glob.h"


void convolution(DynMatr &grid, DynMatr &conv, Config &config) {
    real sum;

    for (auto pt = 0; pt < 2 * config.n_rho + 1; ++pt) {
        for (auto dir = 0; dir < config.n_phi; ++dir) {
            sum = 0;
            for (auto pti = 0; pti < 2 * config.n_rho + 1; ++pti) {
                sum += grid[pti][dir] / (1 - 4 * (pt - pti) * (pt - pti));
            }
            conv[pt][dir] = sum;
        }
    }
}

real backprojection(real x, real y, DynMatr &conv, Config &config) {
    real sum = 0;

    for (auto phi_idx = 0; phi_idx < config.n_phi; ++phi_idx) {
        real phi = PI * phi_idx / config.n_phi;
        real rho = x * cos(phi) + y * sin(phi);
        real rho_real_idx = rho * config.n_rho;
        int rho_idx = floor(rho_real_idx);
        real rho_mantissa = abs(rho_real_idx - rho_idx);

        sum += (1 - rho_mantissa) * conv[config.n_rho + rho_idx][phi_idx] +
               rho_mantissa * conv[config.n_rho + rho_idx + 1][phi_idx];
    }

    return 2 * config.n_rho * sum / (PI * config.n_phi);
}

