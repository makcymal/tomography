#pragma once

#include <functional>
#include "glob.h"
#include "geom.h"
#include "util.h"

using namespace std;
using namespace D2;

DynMatr radon(Area &prm, Config &config);

real backproj(Pnt &pnt, float rho, DynMatr &radon_tr, Config &config);
