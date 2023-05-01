#pragma once

#include "glob.h"
#include "geom.h"

using namespace std;
using namespace D2;

// константа
Area areagen0() {
    Area area = Area(CONST_ATTEN(1));
    return area;
}

// один треугольник и один эллипс
Area areagen1() {
    vector<Ellipse> ellipses = {Ellipse(Pnt(0.25, -0.25), 0.5, 0.25, PI / 4, INV_RADIUS_ATTEN(255))};
    array<Pnt, 3> vertices = {Pnt(-0.5, 0), Pnt(0, 0.5), Pnt(-0.5, -0.5)};
    vector<Polygon> polygons = {Polygon(vertices, RADIUS_ATTEN(255))};
    Area area = Area(OX_ATTEN(255), ellipses, polygons);
    return area;
}
