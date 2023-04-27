#pragma once

#include "glob.h"
#include "util.h"


void convolution(Matrix &, Matrix &, Config &);

float backprojection(double &backproj, double &x, double &y, Matrix &conv, Config &config);
