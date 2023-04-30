#pragma once

#include <functional>
#include "glob.h"

using namespace std;


enum class QuadFormula {
    Rect, Trapeze, Simpson, Gauss2, Gauss3, Gauss4,
};

real quad_formula(function<real(real)> &func, real lb, real rb, QuadFormula formula=QuadFormula::Simpson);

DynList splitting(List(2) inv, int order);
real quadrature_rec(function<real(real)> &func, List(2) inv, int req_order, int cur_order = 0);
real quadrature(function<real(real)> &func, List(2) inv, real accuracy);
real quadrature(function<real(real)> &func, List(2) inv, int order=5, QuadFormula formula=QuadFormula::Simpson);