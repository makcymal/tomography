#include <iostream>
#include <cmath>
#include "radon.h"
#include "nummeth.h"

using namespace std;


DynMatr radon(D2::Area &area, Config &config) {
    DynMatr radon_tr(2 * config.n_rho + 1, vector<real>(config.n_phi, 0));

    for (int rho_idx = -config.n_rho; rho_idx <= config.n_rho; ++rho_idx) {
        real rho = (real)rho_idx / config.n_rho;

        for (int phi_idx = 0; phi_idx < config.n_phi; ++phi_idx) {
            real phi = PI * phi_idx / config.n_phi;
            real cs = cos(phi), sn = sin(phi);

            function<real(real)> func = [&area, rho, cs, sn](real prm) {
                D2::Pnt pnt = {rho * cs - prm * sn, rho * sn + prm * cs};
                return area.attenuation(pnt);
            };

            DynList spltng = splitting({-1, 1}, 9);
            radon_tr.at(rho_idx + config.n_rho).at(phi_idx) = quadrature(func, spltng);
        }
    }
    return radon_tr;
}

real backproj(Pnt &pnt, real r, DynMatr &radon_tr, Config &config) {
    function<real(real)> radon = [r, pnt, &radon_tr](real phi) {
        real rho = r + pnt.x * cos(phi) + pnt.y * sin(phi);
        real tau

        return 0.0;
    };
}
