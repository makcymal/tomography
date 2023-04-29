#pragma once

#include "glob.h"
#include "util.h"


void convolution(DynMatr &, DynMatr &, Config &);

real backprojection(real x, real y, DynMatr &conv, Config &config);
