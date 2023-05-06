#pragma once

#include <functional>
#include "glob.h"

using namespace std;


enum class QuadFormula {
    Rect, Trapeze, Simpson, Gauss2, Gauss3, Gauss4,
};

real quad_formula(function<real(real)> &func, real lb, real rb, QuadFormula formula = QuadFormula::Simpson);

Vector splitting(Array(2) inv, int order);

real quadrature(function<real(real)> &func, Vector spltng, QuadFormula formula = QuadFormula::Simpson);

real quadrature(function<real(real)> &func, Array(2) inv, int order = 5, QuadFormula formula = QuadFormula::Simpson);
