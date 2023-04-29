#pragma once

#include <limits>

#define real float

#define PI 3.1415926535897932

#define CONFIG_FILE "config.env"
#define GRID_FILE "grid.dat"
#define BACKPROJ_FILE "backproj.dat"

#define EPSILON std::numeric_limits<real>::epsilon()
#define real_eq(lhs, rhs) (std::abs(lhs - rhs) < EPSILON)

#define List std::vector<real>
#define Matrix std::vector<std::vector<real>>
