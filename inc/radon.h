#pragma once

#include <functional>
#include "glob.h"
#include "geom.h"
#include "util.h"

using namespace std;
using namespace D2;

DynMatr radon(Area &prm, Config &config);

DynMatr convolution(DynMatr &radon_im, Config &config);

real backprojection(Pnt &pnt, DynMatr &conv, Config &config);

DynMatr inv_radon(DynMatr &radon_im, Config &config);
