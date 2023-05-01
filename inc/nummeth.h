#pragma once

#include <functional>
#include "glob.h"

using namespace std;


enum class QuadFormula {
    Rect, Trapeze, Simpson, Gauss2, Gauss3, Gauss4,
};

real quad_formula(function<real(real)> &func, real lb, real rb, QuadFormula formula = QuadFormula::Simpson);

DynList splitting(List(2) inv, int order);

real quadrature(function<real(real)> &func, DynList spltng, QuadFormula formula = QuadFormula::Simpson);

real quadrature(function<real(real)> &func, List(2) inv, int order = 5, QuadFormula formula = QuadFormula::Simpson);
