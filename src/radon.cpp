#include <iostream>
#include <cmath>
#include "radon.h"
#include "nummeth.h"

using namespace std;

#define QUADORD 7


DynMatr radon(D2::Area &area, Config &config) {
    DynMatr radon_im(2 * config.n_rho + 1, vector<real>(config.n_phi, 0));

    for (int rho_idx = -config.n_rho; rho_idx <= config.n_rho; ++rho_idx) {
        real rho = (real) rho_idx / config.n_rho;

        for (int phi_idx = 0; phi_idx < config.n_phi; ++phi_idx) {
            real phi = PI * phi_idx / config.n_phi;
            real cs = cos(phi), sn = sin(phi);

            function<real(real)> func = [&area, rho, cs, sn](real prm) {
                D2::Pnt pnt = {rho * cs - prm * sn, rho * sn + prm * cs};
                return area.attenuation(pnt, false);
            };

            static DynList spltng = splitting({-1, 1}, QUADORD);
            radon_im.at(rho_idx + config.n_rho).at(phi_idx) = quadrature(func, spltng);
        }
    }
    return radon_im;
}

DynMatr convolution(DynMatr &radon_im, Config &config) {
    DynMatr conv(2 * config.n_rho + 1, vector<real>(config.n_phi, 0));

    for (int rho = 0; rho < 2 * config.n_rho + 1; ++rho) {
        for (int phi = 0; phi < config.n_phi; ++phi) {
            for (int rhoi = 0; rhoi < 2 * config.n_rho + 1; ++rhoi) {
                conv.at(rho).at(phi) +=
                        radon_im.at(rhoi).at(phi) / (real) (1 - 4 * (rho - rhoi) * (rho - rhoi));
            }
        }
    }
    return conv;
}

real backprojection(Pnt &pnt, DynMatr &conv, Config &config) {
    real sum = 0;

    for (int phi_idx = 0; phi_idx < config.n_phi; ++phi_idx) {
        real phi = PI * phi_idx / config.n_phi;
        real rho = pnt.x * cos(phi) + pnt.y * sin(phi);
        int rho_idx = floor(rho * config.n_rho);
        real t_rho = abs(rho * config.n_rho - rho_idx);

        sum += (1 - t_rho) * conv.at(config.n_rho + rho_idx).at(phi_idx) +
               t_rho * conv.at(config.n_rho + rho_idx).at(phi_idx);
    }
    return (real) 2 * config.n_rho * sum / PI / config.n_phi;
}

DynMatr inv_radon(DynMatr &radon_im, Config &config) {
    DynMatr conv = convolution(radon_im, config);
    DynMatr area_obt(2 * config.n_y + 1, vector<real>(2 * config.n_x + 1, 0));

    for (int y_idx = -config.n_y; y_idx <= config.n_y; ++y_idx) {
        real y = (real) -y_idx / config.n_y;
        for (int x_idx = -config.n_x; x_idx <= config.n_x; ++x_idx) {
            Pnt pnt((real) x_idx / config.n_x, y);
            if (pnt.sqrad() > 1) {
                area_obt.at(y_idx + config.n_y).at(x_idx + config.n_x) = 0;
            } else {
                area_obt.at(y_idx + config.n_y).at(x_idx + config.n_x) =
                        max(backprojection(pnt, conv, config), (real) 0);
            }
        }
    }
    return area_obt;
}
