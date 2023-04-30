#pragma GCC optimize("O3")

#include <iostream>
#include <vector>
#include <cmath>
#include "glob.h"
#include "util.h"
#include "radon.h"
#include "nummeth.h"

using namespace std;
using namespace D2;


int main() {
    auto config = Config();

    Area area = Area(CONST_ATTEN(255));

    DynMatr transformed = radon(area, config);
    make_jpg("img/tran.jpg", transformed);

    return 0;
}



