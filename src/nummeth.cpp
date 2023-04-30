#include <iostream>
#include "nummeth.h"
#include "glob.h"

using namespace std;


real quad_formula(function<real(real)> &func, real lb, real rb, QuadFormula formula) {
    real hl = (rb - lb) / 2;
    real md = lb + hl;

    auto from_master = [hl, lb](auto xi) { return hl * (xi + 1) + lb; };

    switch (formula) {
        case QuadFormula::Rect:
            return 2 * hl * func(md);

        case QuadFormula::Trapeze:
            return hl * (func(lb) + func(rb));

        case QuadFormula::Simpson:
            return hl * (func(lb) + 4 * func(md) + func(rb)) / 3;

        case QuadFormula::Gauss2:
            return hl * (func(from_master(-0.57735f)) + func(from_master(0.57735f)));

        case QuadFormula::Gauss3:
            return hl * (5 * func(from_master(-0.774597f)) +
                         8 * func(md) +
                         5 * func(from_master(0.774597f))) / 9;

        case QuadFormula::Gauss4:
            return hl * (23.477225 * func(from_master(-0.339981f)) +
                         23.477225 * func(from_master(0.339981f)) +
                         12.522774 * func(from_master(-0.861136f)) +
                         12.522774 * func(from_master(0.861136f))) / 36;
    }
    return 0;
}


DynList splitting(List(2) inv, int order) {
    // сколько точек осталось отметить
    int rem_pts = (1 << order) + 1;
    DynList splitting(rem_pts, 0);
    splitting.front() = inv[0];
    splitting.back() = inv[1];

    // сколько точек будет отмечено на этой итерации
    int cur_pts = 1;
    for (int ord = 0; ord < order; ++ord) {
        int step = rem_pts >> 1;
        for (int cnt = 0, i = step; cnt < cur_pts; ++cnt, i += rem_pts) {
            splitting.at(i) = (splitting.at(i - step) + splitting.at(i + step)) / 2;
        }
        cur_pts <<= 1;
        rem_pts >>= 1;
    }
    return splitting;
}

real quadrature_rec(function<real(real)> &func, List(2) inv, int req_order, int cur_order) {
    if (cur_order >= req_order) {
        return quad_formula(func, inv[0], inv[1]);
    }
    real mid = (inv[0] + inv[1]) / 2;
    return quadrature_rec(func, {inv[0], mid}, req_order, cur_order + 1) +
           quadrature_rec(func, {mid, inv[1]}, req_order, cur_order + 1);
}

real quadrature(function<real(real)> &func, List(2) inv, real accuracy) {
    DynList attempts;
    attempts.push_back(quad_formula(func, inv[0], inv[1]));
    int iter = 0;

    do {
        iter += 1;
        attempts.push_back(quadrature_rec(func, inv, iter));
    } while (abs(attempts[iter] - attempts[iter - 1]) >= accuracy);

    return attempts[iter];
}

real quadrature(function<real(real)> &func, List(2) inv, int order, QuadFormula formula) {
    if (order < 1) order = 1;
    DynList spltng = splitting(inv, order);
    real quad = 0;
    for (int i = 1; i < spltng.size(); ++i) {
        quad += quad_formula(func, spltng.at(i - 1), spltng.at(i), formula);
    }
    return quad;
}
