#pragma GCC optimize("O3")

#include <iostream>
#include <vector>
#include <cmath>
#include "glob.h"
#include "util.h"
#include "radon.h"
#include "nummeth.h"
#include "area_gen.h"

using namespace std;
using namespace D2;


int main() {
    Config config = Config();

    auto area = areagen1();
    area.image(config);

    DynMatr radon_im = radon(area, config);
    make_jpg("img/radon_im.jpg", radon_im);

    DynMatr area_obt = inv_radon(radon_im, config);
    make_jpg("img/area_obt.jpg", area_obt);

    return 0;
}



