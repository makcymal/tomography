#include <iostream>
#include <array>
#include <cmath>
#include <cassert>

#include "geom.h"
#include "glob.h"
#include "util.h"

using namespace std;

#ifndef STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#endif
#include "stb_image_write.h"


// Конструктор точки
D2::Pnt::Pnt(real x, real y) :
        x(x), y(y) {}

// Квадрат расстояния до точки из начала координат
real D2::Pnt::sqrad() const {
    return x * x + y * y;
}

// Проверка на действительность
bool D2::Pnt::is_nopnt() const {
    return x == D2::NOPNT.x && y == D2::NOPNT.y;
}


// Конструктор вектора по зенитному углу
D2::Vec::Vec(real azimuth) :
        azicos(cos(azimuth)), azisin(sin(azimuth)) {}

// Конструктор вектора по значениям косинуса и синуса
D2::Vec::Vec(real azicos, real azisin) :
        azicos(azicos), azisin(azisin) {}

// Вектор, перпендикулярный данному
D2::Vec D2::Vec::perp() const {
    return {-azisin, azicos};
}

// Направляющие косинусы
List(2) D2::Vec::guiding_cos() const {
    return {azicos, azisin};
}

// Проверка на действительность
bool D2::Vec::is_novec() const {
    return azicos == D2::NOVEC.azicos && azisin == D2::NOVEC.azisin;
}


// Конструктор луча
D2::Ray::Ray(Pnt inception, Vec direction) :
        inception(inception), direction(direction) {}

// Луч, перпендикулярный данному
D2::Ray D2::Ray::perp() const {
    return {inception, direction.perp()};
}

// Проверка на действительность
bool D2::Ray::is_noray() const {
    return inception.is_nopnt() && direction.is_novec();
}


// Конструктор эллипса
D2::Ellipse::Ellipse(Pnt center, real a, real b, real rotation, function<real(const Pnt &)> attenuation) :
        center(center),
        sqa(a * a), sqb(b * b),
        rotcos(cos(rotation)), rotsin(sin(rotation)),
        attenuation(std::move(attenuation)) {}

// Формула для графопостроителя
void D2::Ellipse::formula() const {
    cout << sqb << "(" << rotcos << "(x-" << center.x << ") + " << rotsin << "(y-" << center.y << "))^2 + "
         << sqa << "(" << rotsin << "(x-" << center.x << ") - " << rotcos << "(y-" << center.y << "))^2"
         << " = " << sqa * sqb << endl;
}

// Проверка на вхождение точки
bool D2::Ellipse::contains(Pnt &pnt) const {
    real
            xi = rotcos * (pnt.x - center.x) + rotsin * (pnt.y - center.y),
            nu = rotsin * (pnt.x - center.x) - rotcos * (pnt.y - center.y);
    return sqb * xi * xi + sqa * nu * nu <= sqa * sqb;
}

// Точки пересечения с заданным лучом
array<D2::Pnt, 2> D2::Ellipse::collide(Ray &ray) const {
#define ac ray.direction.azicos
#define as ray.direction.azisin

    real
            dx = ray.inception.x - center.x,
            dy = ray.inception.y - center.y;
    real
            p = sqb * rotcos * rotcos + sqa * rotsin * rotsin,
            q = sqb * rotsin * rotsin + sqa * rotcos * rotcos,
            g = sqb - sqa;

    // Коэффициенты квадратного уравнения относительно расстояния между началом луча и точкой пересечения с эллипсом
    real A = ac * ac * p + 2 * rotcos * rotsin * ac * as * g + as * as * q;
    real B = 2 * dx * ac * p + 2 * rotcos * rotsin * g * (dx * as + dy * ac) + 2 * dy * as * q;
    real C = dx * dx * p + 2 * dx * dy * rotcos * rotsin * g + dy * dy * q - sqa * sqb;

    array<Pnt, 2> collisions = {NOPNT, NOPNT};
    List(2) tau = quadeq(A, B, C);

    for (auto i = 0; i < 2; ++i) {
        if (tau[i] < 0 || tau[i] == NOREAL) return collisions;
        collisions[i] = Pnt(ray.inception.x + ac * tau[i], ray.inception.y + as * tau[i]);
    }
    return collisions;

#undef ac
#undef as
}


// Конструктор треугольника
D2::Polygon::Polygon(array<Pnt, 3> vertices, function<real(const Pnt &)> attenuation) :
        vertices(vertices),
        attenuation(std::move(attenuation)) {}

// Проверка на вхождение точки
bool D2::Polygon::contains(Pnt &pnt) const {
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

// Точки пересечения с заданным лучом
array<D2::Pnt, 2> D2::Polygon::collide(Ray &ray) const {
#define ac ray.direction.azicos
#define as ray.direction.azisin

    array<D2::Pnt, 2> collisions = {NOPNT, NOPNT};
    int found = 0;
    int last_idx = -1;
    real prev_t = -1;

    for (auto i = 0; i < 3; ++i) {
        // Если две точки столкновения уже найдены
        if (found == 2) return collisions;

        const Pnt &lend = vertices[i % 3];
        const Pnt &rend = vertices[(i + 1) % 3];

        // Пусть искомая точка: lend * t + rend * (1 - t) = ray.inception + l * (zencos, zensin)
        // Определитель матрицы системы на t, l
        real det = as * (rend.x - lend.x) + ac * (lend.y - rend.y);
        if (real_eq(det, 0)) {
            prev_t = -1;
            continue;
        }

        real t = (as * (rend.x - ray.inception.x) + ac * (ray.inception.y - rend.y)) / det;
        real l = ((rend.y - lend.y) * ray.inception.x + (lend.x - rend.x) * ray.inception.y +
                  lend.y * rend.x - lend.x * rend.y) / det;
        if (t < 0 || 1 < t || real_eq(prev_t + t, 1) || l < 0) {
            prev_t = -1;
            continue;
        }

        collisions[found] = {lend.x * t + rend.x * (1 - t), lend.y * t + rend.y * (1 - t)};
        found++;
        prev_t = t;
    }

    return collisions;

#undef ac
#undef as
}


// Конструктор области
D2::Area::Area(vector<Polygon> polygons, vector<Ellipse> ellipses, function<real(const Pnt &)> attenuation)
        : polygons(std::move(polygons)),
          ellipses(std::move(ellipses)),
          attenuation(std::move(attenuation)) {}


// Создание изображения "area.jpg" с полутоновым изображением области
void D2::Area::image(int size) {
    unsigned char atten[size][size];
    real step = 2.f / (real) size;
    Pnt pnt = {-1, 1};

    for (auto y = 0; y < size; ++y) {
        for (auto x = 0; x < size; ++x) {
            bool contained = false;

            if (pnt.sqrad() > 1) {
                atten[y][x] = 0;
                contained = true;
            }
            for (const auto &polygon: polygons) {
                if (contained) break;
                if (polygon.contains(pnt)) {
                    atten[y][x] = (unsigned char) polygon.attenuation(pnt);
                    contained = true;
                }
            }
            for (const auto &ellipse: ellipses) {
                if (contained) break;
                if (ellipse.contains(pnt)) {
                    atten[y][x] = (unsigned char) ellipse.attenuation(pnt);
                    contained = true;
                }
            }
            if (!contained) {
                atten[y][x] = (unsigned char) attenuation(pnt);
            }
            pnt.x += step;
        }
        pnt.y -= step;
        pnt.x = -1;
    }

    stbi_write_jpg("img/area.jpg", size, size, 1, atten, 100);
}


// Конструктор точки
D3::Pnt::Pnt(real x, real y, real z) :
        x(x), y(y), z(z) {}

// Квадрат расстояния от начала координат до точки
real D3::Pnt::sqrad() const {
    return x * x + y * y + z * z;
}

// Проверка на действительность
bool D3::Pnt::is_nopnt() const {
    return x == D3::NOPNT.x && y == D3::NOPNT.y && z == D3::NOPNT.z;
}


// Конструктор вектора по зенитному и азимутальному углу
D3::Vec::Vec(real azimuth, real zenith) :
        azicos(cos(azimuth)), azisin(sin(azimuth)),
        zencos(cos(zenith)), zensin(sin(zenith)) {}

// Конструктор вектора по значениям косинусов и синусов
D3::Vec::Vec(real azicos, real azisin, real zencos, real zensin) :
        azicos(azicos), azisin(azisin),
        zencos(zencos), zensin(zensin) {}

// Вектор, перпендикулярный данному
D3::Vec D3::Vec::perp() const {
    return {-azisin, azicos, 0, 1};
}

// Направляющие косинусы
List(3) D3::Vec::guiding_cos() const {
    return {zencos * azicos, zencos * azisin, zensin};
}

// Проверка на действительность
bool D3::Vec::is_novec() const {
    return azicos == D3::NOVEC.azicos && azisin == D3::NOVEC.azisin &&
           zencos == D3::NOVEC.zencos && zensin == D3::NOVEC.zensin;
}

// Конструктор луча
D3::Ray::Ray(Pnt inception, Vec direction) :
        inception(inception), direction(direction) {}

// Луч, перпендикулярный данному
D3::Ray D3::Ray::perp() const {
    return {inception, direction.perp()};
}

// Проверка на действительность
bool D3::Ray::is_noray() const {
    return inception.is_nopnt() && direction.is_novec();
}


// Точки пересечения с заданным лучом
array<D3::Pnt, 2> D3::Sphere::collide(Ray &ray) {
    real C = ray.inception.sqrad();
    assert(C <= 1);

    List(3) guiding_cos = ray.direction.guiding_cos();
    real B = ray.inception.x * guiding_cos[0] +
             ray.inception.y * guiding_cos[1] +
             ray.inception.z * guiding_cos[2];

    // корни в порядке убывания
    List(2) tau = quadeq(1, B, C);
    array<Pnt, 2> collisions = {NOPNT, NOPNT};

    for (auto i = 0; i < 2; ++i) {
        if (tau[i] == NOREAL) break;
        collisions[i] = {
                ray.inception.x + tau[i] * guiding_cos[0],
                ray.inception.y + tau[i] * guiding_cos[1],
                ray.inception.z + tau[i] * guiding_cos[2]
        };
    }
    return collisions;
}
