#pragma GCC optimize("O3")

#include <iostream>
#include <vector>

using namespace std;

#include "geom.h"
#include "glob.h"


int main() {
    array<D2::Pnt, 3> vertices = {D2::Pnt(-0.5, 0), D2::Pnt{0, 0.5}, D2::Pnt{-0.5, -0.5}};
    vector<D2::Polygon> polygons;
    polygons.emplace_back(vertices, CATHETS_ATTEN(254.f));

    D2::Pnt center = {0.25, -0.25};
    vector<D2::Ellipse> ellipses;
    ellipses.emplace_back(center, 0.5, 0.25, PI / 4, INV_RADIUS_ATTEN(255));

    D2::Area area = {polygons, ellipses, OY_ATTEN(255)};
    area.image(1000);

    return 0;
}
