#include <iostream>
#include <cmath>
#include "radon.h"
#include "nummeth.h"

using namespace std;


DynMatr radon(D2::Area &area, Config &config) {
    DynMatr radon_tr(config.n_rho + 1, vector<real>(config.n_phi, 0));

    for (int rho_idx = 0; rho_idx <= config.n_rho; ++rho_idx) {
        real rho = (real) rho_idx / config.n_rho;

        for (int phi_idx = 0; phi_idx < config.n_phi; ++phi_idx) {
            real phi = DPI * phi_idx / config.n_phi;
            real cs = cos(phi), sn = sin(phi);

            function<real(real)> func = [&area, rho, cs, sn](real prm) {
                D2::Pnt pnt = {rho * cs - prm * sn, rho * sn + prm * cs};
                return area.attenuation(pnt);
            };

            static DynList spltng = splitting({-1, 1}, 7);
            radon_tr.at(rho_idx).at(phi_idx) = quadrature(func, spltng);
        }
    }
    return radon_tr;
}

real backproj(Pnt &pnt, real r, DynMatr &radon_tr, Config &config) {
    static DynList spltng = splitting({0, DPI}, 7);
    function<real(real)> radon = [r, pnt, &radon_tr, &config](real phi) {
        real rho = r + pnt.x * cos(phi) + pnt.y * sin(phi);
        if (rho > 1) return (real)0;

        if (rho < 0) {
            if (phi > PI) phi -= PI;
            else phi += PI;
            rho *= -1;
        }

        int rho_idx = floor(rho * config.n_rho), phi_idx = floor(phi * config.n_phi);
        real t_rho = rho * config.n_rho - rho_idx, t_phi = phi * config.n_phi - phi_idx;

        return (1 - t_phi) * ((1 - t_rho) * radon_tr.at(rho_idx).at(phi_idx) +
                              t_rho * radon_tr.at(rho_idx + 1).at(phi_idx)) +
               t_phi * ((1 - t_rho) * radon_tr.at(rho_idx).at(phi_idx + 1) +
                              t_rho * radon_tr.at(rho_idx + 1).at(phi_idx + 1));
    };
    return quadrature(radon, spltng) / DPI;
}
