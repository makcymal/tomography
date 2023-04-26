#pragma once

#include <limits>

#define PI 3.1415926535897932

#define CONFIG_FILE "config.env"
#define GRID_FILE "grid.dat"
#define BACKPROJ_FILE "backproj.dat"

#define EPSILON std::numeric_limits<float>::epsilon()
#define NOFLT std::numeric_limits<float>::lowest()

#define FLT_EQ(lhs, rhs) (std::abs(lhs - rhs) < EPSILON)
#define FLT_GT(lhs, rhs) (std::abs(lhs - rhs) >= EPSILON && lhs > rhs)

#define Matrix vector<vector<double>>
