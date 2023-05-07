#pragma once

#include <limits>

#define real double

#define PI 3.1415926535897932384626433832795028841971693993751058209749445923
#define DPI 6.2831853071795864769252867665590057683943387987502116419498891846

#define CONFIG_FILE "config.env"

#define EPSILON std::numeric_limits<real>::epsilon()
#define real_eq(lhs, rhs) (std::abs(lhs - rhs) < EPSILON)

#define Array(len) std::array<real, len>
#define Vector std::vector<real>
#define Matrix std::vector<std::vector<real>>
