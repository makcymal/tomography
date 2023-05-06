#pragma GCC optimize("O3")

#include <vector>
#include <string>
#include "radon.h"
#include "area_gen.h"

using namespace std;


int main() {
    Config config = Config();
    string suffix("_shepplogan_"), filename;
    suffix = (suffix + to_string(config.n_rho)).append("_") + to_string(config.n_phi);

    auto area = area_head();
    Matrix area_im = area.image(config, true);
    filename = string("area_giv") + suffix;
    make_jpg_dat(filename, area_im);

    Matrix radon_im = radon(area, config);
    filename = string("radon_im") + suffix;
    make_jpg_dat(filename, radon_im);

    Matrix area_obt = inv_radon(radon_im, config);
    filename = string("area_obt") + suffix;
    make_jpg_dat(filename, area_obt);

    return 0;
}
