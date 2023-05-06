#include <iostream>
#include <array>
#include <cmath>
#include <cassert>
#include <utility>

#include "geom.h"
#include "glob.h"

using namespace std;


namespace D2 {
    // Конструктор точки
    Pnt::Pnt(real x, real y) :
            x(x), y(y) {}

    // Квадрат расстояния до точки из начала координат
    real Pnt::sqrad() const {
        return x * x + y * y;
    }


    // Конструктор вектора по зенитному углу
    Vec::Vec(real azimuth) :
            azicos(cos(azimuth)), azisin(sin(azimuth)) {}

    // Конструктор вектора по значениям косинуса и синуса
    Vec::Vec(real azicos, real azisin) :
            azicos(azicos), azisin(azisin) {}

    // Вектор, перпендикулярный данному
    Vec Vec::perp() const {
        return {-azisin, azicos};
    }

    // Направляющие косинусы
    List(2) Vec::guiding_cos() const {
        return {azicos, azisin};
    }


    // Конструктор луча
    Ray::Ray(Pnt inception, Vec direction) :
            inception(inception), direction(direction) {}

    // Луч, перпендикулярный данному
    Ray Ray::perp() const {
        return {inception, direction.perp()};
    }


    // Конструктор эллипса
    Ellipse::Ellipse(Pnt center, real a, real b, real rotation, function<real(const Pnt &)> atten) :
            center(center),
            sqa(a * a), sqb(b * b),
            rotcos(cos(rotation)), rotsin(sin(rotation)),
            atten(std::move(atten)) {}

    // Формула для графопостроителя
    void Ellipse::formula() const {
        cout << sqb << "(" << rotcos << "(x-" << center.x << ") + " << rotsin << "(y-" << center.y << "))^2 + "
             << sqa << "(" << rotsin << "(x-" << center.x << ") - " << rotcos << "(y-" << center.y << "))^2"
             << " = " << sqa * sqb << endl;
    }

    // Проверка на вхождение точки
    bool Ellipse::contains(Pnt &pnt) const {
        real
                xi = rotcos * (pnt.x - center.x) + rotsin * (pnt.y - center.y),
                nu = rotsin * (pnt.x - center.x) - rotcos * (pnt.y - center.y);
        return sqb * xi * xi + sqa * nu * nu <= sqa * sqb;
    }

    // Точки пересечения с заданным лучом
    vector<Pnt> Ellipse::collide(Ray &ray) const {
        real
                ac = ray.direction.azicos,
                as = ray.direction.azisin,

                dx = ray.inception.x - center.x,
                dy = ray.inception.y - center.y,

                p = sqb * rotcos * rotcos + sqa * rotsin * rotsin,
                q = sqb * rotsin * rotsin + sqa * rotcos * rotcos,
                g = sqb - sqa;

        // Коэффициенты квадратного уравнения относительно расстояния между началом луча и точкой пересечения с эллипсом
        real
                A = ac * ac * p + 2 * rotcos * rotsin * ac * as * g + as * as * q,
                B = 2 * dx * ac * p + 2 * rotcos * rotsin * g * (dx * as + dy * ac) + 2 * dy * as * q,
                C = dx * dx * p + 2 * dx * dy * rotcos * rotsin * g + dy * dy * q - sqa * sqb;

        vector<Pnt> collisions;
        DynList tau = quadeq(A, B, C);

        for (auto i = 0; i < 2; ++i) {
            if (tau[i] < 0) return collisions;
            collisions.emplace_back(ray.inception.x + ac * tau[i], ray.inception.y + as * tau[i]);
        }
        return collisions;
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

    // Точки пересечения с заданным лучом
    vector<Pnt> Polygon::collide(Ray &ray) const {
        real
                ac = ray.direction.azicos,
                as = ray.direction.azisin,
                prev_t = -1;

        vector<Pnt> collisions;

        for (auto i = 0; i < 3; ++i) {
            // Если две точки столкновения уже найдены
            if (collisions.size() == 2) return collisions;

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

            collisions.emplace_back(lend.x * t + rend.x * (1 - t), lend.y * t + rend.y * (1 - t));
            prev_t = t;
        }

        return collisions;
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

    // Создание изображения "area.jpg" с полутоновым изображением области
    void Area::image(Config &config, bool intersections) const {
        int height = 2 * config.n_y + 1, width = 2 * config.n_x + 1;
        DynMatr pixels(height, vector<real>(width));
        real dy = (real) 2 / height, dx = (real) 2 / width;
        Pnt pnt = {(real) -1, (real) 1};

        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                pixels[y][x] = attenuation(pnt, intersections);
                pnt.x += dx;
            }
            pnt.y -= dy;
            pnt.x = -1;
        }
        make_jpg("img/area.jpg", pixels);
    }
}


namespace D3 {
    // Конструктор точки
    Pnt::Pnt(real x, real y, real z) :
            x(x), y(y), z(z) {}

    // Квадрат расстояния от начала координат до точки
    real Pnt::sqrad() const {
        return x * x + y * y + z * z;
    }


    // Конструктор вектора по зенитному и азимутальному углу
    Vec::Vec(real azimuth, real zenith) :
            azicos(cos(azimuth)), azisin(sin(azimuth)),
            zencos(cos(zenith)), zensin(sin(zenith)) {}

    // Конструктор вектора по значениям косинусов и синусов
    Vec::Vec(real azicos, real azisin, real zencos, real zensin) :
            azicos(azicos), azisin(azisin),
            zencos(zencos), zensin(zensin) {}

    // Вектор, перпендикулярный данному
    Vec Vec::perp() const {
        return {-azisin, azicos, 0, 1};
    }

    // Направляющие косинусы
    List(3) Vec::guiding_cos() const {
        return {zencos * azicos, zencos * azisin, zensin};
    }


    // Конструктор луча
    Ray::Ray(Pnt inception, Vec direction) :
            inception(inception), direction(direction) {}

    // Луч, перпендикулярный данному
    Ray Ray::perp() const {
        return {inception, direction.perp()};
    }


    // Точки пересечения с заданным лучом
    vector<Pnt> Sphere::collide(Ray &ray) {
        real C = ray.inception.sqrad();
        assert(C <= 1);

        List(3) guiding_cos = ray.direction.guiding_cos();
        real B = ray.inception.x * guiding_cos[0] +
                 ray.inception.y * guiding_cos[1] +
                 ray.inception.z * guiding_cos[2];

        // корни в порядке убывания
        DynList tau = quadeq(1, B, C);
        vector<Pnt> collisions;

        for (auto i = 0; i < tau.size(); ++i) {
            collisions[i] = {
                    ray.inception.x + tau[i] * guiding_cos[0],
                    ray.inception.y + tau[i] * guiding_cos[1],
                    ray.inception.z + tau[i] * guiding_cos[2]
            };
        }
        return collisions;
    }
}



