#include <iostream>
#include <cmath>
#include "radon.h"
#include "nummeth.h"

using namespace std;

#define QUADORD 6


DynMatr radon(D2::Area &area, Config &config) {
    DynMatr radon_im(2 * config.n_rho + 1, vector<real>(config.n_phi, 0));

    for (int rho_idx = -config.n_rho; rho_idx <= config.n_rho; ++rho_idx) {
        real rho = (real) rho_idx / config.n_rho;

        for (int phi_idx = 0; phi_idx < config.n_phi; ++phi_idx) {
            real phi = PI * phi_idx / config.n_phi;
            real cs = cos(phi), sn = sin(phi);

            function<real(real)> func = [&area, rho, cs, sn](real prm) {
                D2::Pnt pnt = {rho * cs - prm * sn, rho * sn + prm * cs};
                return area.attenuation(pnt);
            };

            static DynList spltng = splitting({-1, 1}, QUADORD);
            radon_im.at(rho_idx + config.n_rho).at(phi_idx) = quadrature(func, spltng);
        }
    }
    return radon_im;
}

real backproj(const Pnt &pnt, real r, DynMatr &radon_im, Config &config) {
    static DynList spltng = splitting({0, DPI - (PI / (1 << (QUADORD - 1)))}, QUADORD);

    function<real(real)> radon = [r, pnt, &radon_im, &config](real phi) {
        real rho = r + pnt.x * cos(phi) + pnt.y * sin(phi);
        if (abs(rho) >= 1) return (real) 0;

        phi /= PI;
        if (phi >= 1) {
            rho *= -1;
            phi -= 1;
        }
        // -1 <= rho <= 1   ,   0 <= phi < 1

        int rho_idx = floor(rho * config.n_rho), phi_idx = floor(phi * config.n_phi);
        real t_rho = rho * config.n_rho - rho_idx, t_phi = phi * config.n_phi - phi_idx;

        rho_idx += config.n_rho;
        int nxt_rho_idx = rho_idx + 1, nxt_phi_idx = phi_idx + 1;
        if (nxt_rho_idx == 2 * config.n_rho + 1) nxt_rho_idx--;
        if (nxt_phi_idx == config.n_phi) nxt_phi_idx--;

        // двумерное интерполирование
        return (1 - t_phi) * ((1 - t_rho) * radon_im.at(rho_idx).at(phi_idx) +
                              t_rho * radon_im.at(nxt_rho_idx).at(phi_idx)) +
               t_phi * ((1 - t_rho) * radon_im.at(rho_idx).at(nxt_phi_idx) +
                        t_rho * radon_im.at(nxt_rho_idx).at(nxt_phi_idx));
    };

    // если не по ф. трапеций, то отступ от DPI брать больше
    return quadrature(radon, spltng, QuadFormula::Trapeze) / DPI;
}

DynMatr inv_radon(DynMatr &radon_im, Config &config) {
    static DynList spltng = splitting({0, 3}, QUADORD);

    DynMatr area_obt(2 * config.n_y + 1, vector<real>(2 * config.n_x + 1, 0));

    for (int y_idx = -config.n_y; y_idx <= config.n_y; ++y_idx) {
        real y = (real) y_idx / config.n_y;
        cout << y << endl;
        for (int x_idx = -config.n_x; x_idx <= config.n_x; ++x_idx) {
            Pnt pnt((real) x_idx / config.n_x, y);
            real bp0 = backproj(pnt, 0, radon_im, config);

            function<real(real)> func = [&radon_im, &config, pnt, bp0](real r) {
                return (backproj(pnt, r, radon_im, config) - bp0) / r / r;
            };

            real quad = - (quadrature(func, spltng, QuadFormula::Trapeze) + bp0 / 3) / PI;
            area_obt.at(y_idx + config.n_y).at(x_idx + config.n_x) = quad;
        }
    }
    return area_obt;
}
