#include <iostream>
#include <cmath>
#include "radon.h"
#include "nummeth.h"

using namespace std;


DynMatr radon(D2::Area &area, Config &config) {
    DynMatr transformed(2 * config.n_rho + 1, vector<real>(config.n_phi, 0));

    for (int i = -config.n_rho; i <= config.n_rho; ++i) {
        real rho = (real)i / config.n_rho;

        for (int j = 0; j < config.n_phi; ++j) {
            real phi = PI * j / config.n_phi;
            real cs = cos(phi), sn = sin(phi);

            function<real(real)> func = [&area, rho, cs, sn](real prm) {
                D2::Pnt pnt = {rho * cs - prm * sn, rho * sn + prm * cs};
                return area.attenuation(pnt);
            };
            real tran = quadrature(func, {-1, 1}, 9);
            transformed.at(i + config.n_rho).at(j) = tran;
        }
    }
    return transformed;
}