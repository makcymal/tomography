#pragma once

#include <limits>

#define real float

#define PI 3.1415926535897932f
#define DPI 6.283185307179586f

#define CONFIG_FILE "config.env"

#define EPSILON std::numeric_limits<real>::epsilon()
#define real_eq(lhs, rhs) (std::abs(lhs - rhs) < EPSILON)

#define Array(len) std::array<real, len>
#define Vector std::vector<real>
#define Matrix std::vector<std::vector<real>>
