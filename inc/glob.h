#pragma once

#include <limits>

#define real float

#define PI 3.1415926535897932
#define DPI 6.283185307179586

#define CONFIG_FILE "config.env"

#define EPSILON std::numeric_limits<real>::epsilon()
#define real_eq(lhs, rhs) (std::abs(lhs - rhs) < EPSILON)

#define PINF std::numeric_limits<real>::infinity();
#define MINF -std::numeric_limits<real>::infinity();

#define List(len) std::array<real, len>
#define Matr(row, col) std::array<std::array<real, col>, row>
#define DynList std::vector<real>
#define DynMatr std::vector<std::vector<real>>
