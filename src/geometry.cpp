#include <cmath>
#include <fstream>
#include <cstring>
#include "geometry.h"

using namespace std;


// Конструктор точки
Pnt::Pnt(real x, real y) :
        x(x), y(y) {}

// Квадрат расстояния до точки из начала координат
real Pnt::sqrad() const {
    return x * x + y * y;
}


// Конструктор эллипса
Ellipse::Ellipse(Pnt center, real a, real b, real rotation, function<real(const Pnt &)> atten) :
        center(center),
        sqa(a * a), sqb(b * b),
        rotcos(cos(rotation)), rotsin(sin(rotation)),
        atten(std::move(atten)) {}

// Проверка на вхождение точки
bool Ellipse::contains(Pnt &pnt) const {
    real
            xi = rotcos * (pnt.x - center.x) + rotsin * (pnt.y - center.y),
            nu = rotsin * (pnt.x - center.x) - rotcos * (pnt.y - center.y);
    return sqb * xi * xi + sqa * nu * nu <= sqa * sqb;
}

// Коэффиент ослабления внутри эллипса
real Ellipse::attenuation(Pnt &pnt) const {
    return atten(pnt);
}


// Конструктор треугольника
Polygon::Polygon(array<Pnt, 3> vertices, function<real(const Pnt &)> atten) :
        vertices(vertices),
        atten(std::move(atten)) {}

// Проверка на вхождение точки
bool Polygon::contains(Pnt &pnt) const {
    // следующие экземпляры точек имеют смысл радиус-векторов
    Pnt v01 = {vertices[1].x - vertices[0].x, vertices[1].y - vertices[0].y};
    Pnt v02 = {vertices[2].x - vertices[0].x, vertices[2].y - vertices[0].y};
    Pnt v0p = {pnt.x - vertices[0].x, pnt.y - vertices[0].y};

    if ((v01.x * v0p.y - v0p.x * v01.y) * (v02.x * v0p.y - v0p.x * v02.y) > 0) {
        return false;
    }

    Pnt v10 = {vertices[0].x - vertices[1].x, vertices[0].y - vertices[1].y};
    Pnt v12 = {vertices[2].x - vertices[1].x, vertices[2].y - vertices[1].y};
    Pnt v1p = {pnt.x - vertices[1].x, pnt.y - vertices[1].y};

    return (v10.x * v1p.y - v1p.x * v10.y) * (v12.x * v1p.y - v1p.x * v12.y) <= 0;
}

// Коэффиент ослабления внутри треугольника
real Polygon::attenuation(Pnt &pnt) const {
    return atten(pnt);
}


// Конструктор области
Area::Area(function<real(const Pnt &)> atten, vector<Ellipse> ellipses, vector<Polygon> polygons)
        : ellipses(std::move(ellipses)),
          polygons(std::move(polygons)),
          atten(std::move(atten)) {}

// Коэффиент ослабления внутри области с учетом внутренних областей
real Area::attenuation(Pnt &pnt, bool intersections) const {
    if (pnt.sqrad() > 1) {
        return 0;
    }
    real result = atten(pnt);
    for (const auto &ellipse: ellipses) {
        if (ellipse.contains(pnt)) {
            if (intersections) {
                result += ellipse.attenuation(pnt);
            } else {
                return ellipse.attenuation(pnt);
            }
        }
    }
    for (const auto &polygon: polygons) {
        if (polygon.contains(pnt)) {
            if (intersections) {
                result += polygon.attenuation(pnt);
            } else {
                return polygon.attenuation(pnt);
            }
        }
    }
    return result;
}

Matrix Area::image(Config &config) const {
    int height = 2 * config.n_y + 1, width = 2 * config.n_x + 1;
    Matrix image(height, vector<real>(width));
    real dy = (real) 2 / height, dx = (real) 2 / width;
    Pnt pnt = {(real) -1, (real) 1};

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            real value = attenuation(pnt, config.intersections);
            image[y][x] = value;
            pnt.x += dx;
        }
        pnt.y -= dy;
        pnt.x = -1;
    }
    return image;
}
