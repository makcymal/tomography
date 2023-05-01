#pragma once

#include <array>
#include <functional>
#include "glob.h"
#include "util.h"

using namespace std;

namespace D2 {
    // Точка на координатной плоскости
    class Pnt {
    public:
        // Координаты точки
        real x, y;

        // Конструктор точки
        Pnt(real x, real y);

        // Квадрат расстояния до точки из начала координат
        [[nodiscard]] real sqrad() const;
    };


    // Единичный вектор, задается азимутальным в [0, 2pi) углом
    class Vec {
    public:
        // Косинус и синус азимутального угла
        real azicos, azisin;

        // Конструктор вектора по азимутальному углу
        explicit Vec(real azimuth);

        // Конструктор вектора по значениям косинуса и синуса
        Vec(real azicos, real azisin);

        // Вектор, перпендикулярный данному
        [[nodiscard]] Vec perp() const;

        // Направляющие косинусы
        [[nodiscard]] List(2) guiding_cos() const;
    };


    // Луч как закрепленный вектор
    class Ray {
    public:
        // Точка начала луча
        Pnt inception;
        // Направляющий вектор луча
        Vec direction;

        // Конструктор луча
        Ray(Pnt inception, Vec direction);

        // Луч, перпендикулярный данному
        [[nodiscard]] Ray perp() const;
    };


    // Произвольный эллипс на координатной плоскости
    // Задается точкой центра, длинами большой и малой полуоси, азимутальным углом поворота
    class Ellipse {
        // Центр эллипса
        Pnt center;
        // Квадраты длин большой и малой полуосей
        real sqa, sqb;
        // Косинусы и синусы угла поворота
        real rotcos, rotsin;
        // Лямбда, задающая значение коэффициента ослабления внутри эллипса
        function<real(const Pnt &)> atten;
    public:
        // Конструктор эллипса
        Ellipse(Pnt center, real a, real b, real rotation, function<real(const Pnt &)> atten);

        // Формула для графопостроителя
        void formula() const;

        // Проверка на вхождение точки
        [[nodiscard]] bool contains(Pnt &pnt) const;

        // Точки пересечения с заданным лучом
        [[nodiscard]] vector<Pnt> collide(Ray &ray) const;

        // Коэффиент ослабления внутри эллипса
        real attenuation(Pnt &pnt) const;
    };


    // Произвольный треугольник на координатной плоскости, задается точками вершин
    class Polygon {
        // Точки вершин, в порядке по часовой стрелке
        array<Pnt, 3> vertices;
        // Лямбда, задающая значение коэффициента ослабления внутри треугольника
        function<real(const Pnt &)> atten;
    public:
        // Конструктор треугольника
        Polygon(array<Pnt, 3> vertices, function<real(const Pnt &)> atten);

        // Проверка на вхождение точки
        [[nodiscard]] bool contains(Pnt &pnt) const;

        // Точки пересечения с заданным лучом
        [[nodiscard]] vector<Pnt> collide(Ray &ray) const;

        // Коэффиент ослабления внутри треугольника
        real attenuation(Pnt &pnt) const;
    };


    // Область - круг радиуса 1 с центром в начале координат, представляющий собой сечение области G
    class Area {
        // Содержащиеся внутри эллипсы
        vector<Ellipse> ellipses;
        // Содержащиеся внутри треугольники
        vector<Polygon> polygons;
        // Лямбда, задающая значение коэффициента ослабления внутри области
        function<real(const Pnt &)> atten;
    public:
        // Конструктор области
        explicit Area(function<real(const Pnt &)> atten, vector<Ellipse> ellipse={}, vector<Polygon> polygon={});

        // Коэффиент ослабления внутри области с учетом внутренних областей
        real attenuation(Pnt &pnt) const;

        // Создание изображения "area.jpg" с полутоновым изображением области
        void image(Config &config) const;
    };
}

// Функции распределения коэффициента ослабления
#define RADIUS_ATTEN(diap) ([](const D2::Pnt &pnt) { return pnt.sqrad() * diap; })
#define INV_RADIUS_ATTEN(diap) ([](const D2::Pnt &pnt) { return (1 - pnt.sqrad() * diap); })
#define CATHETS_ATTEN(diap) ([](const D2::Pnt &pnt) { return (std::abs(pnt.x) + std::abs(pnt.y)) * (diap / 2); })
#define OX_ATTEN(diap) ([](const D2::Pnt &pnt) { return std::abs(pnt.x) * diap; })
#define OY_ATTEN(diap) ([](const D2::Pnt &pnt) { return std::abs(pnt.y) * diap; })
#define CONST_ATTEN(val) ([](const D2::Pnt &pnt) { return val; })


namespace D3 {
    // Точка в пространстве
    class Pnt {
    public:
        // Координаты точки
        real x, y, z;

        // Конструктор точки
        Pnt(real x, real y, real z);

        // Квадрат расстояния до точки из начала координат
        [[nodiscard]] real sqrad() const;
    };


    // Единичный вектор, задается азимутальным в [0, 2pi) и зенитным в [0, pi] углом
    class Vec {
    public:
        // Косинусы и синусы азимутального и зенитного углов
        real azicos, azisin, zencos, zensin;

        // Конструктор вектора по азимутальному и зенитному углу
        Vec(real azimuth, real zenith);

        // Конструктор вектора по значениям косинусов и синусов
        Vec(real azicos, real azisin, real zencos, real zensin);

        // Вектор, перпендикулярный данному
        [[nodiscard]] Vec perp() const;

        // Направляющие косинусы
        [[nodiscard]] List(3) guiding_cos() const;
    };


    // Луч как закрепленный вектор
    class Ray {
    public:
        // Точка начала луча
        Pnt inception;
        // Направляющий вектор луча
        Vec direction;

        // Конструктор луча
        Ray(Pnt inception, Vec direction);

        // Луч, перпендикулярный данному
        [[nodiscard]] Ray perp() const;
    };


    class Sphere {
    public:
        // Точки пересечения с заданным лучом
        static vector<Pnt> collide(Ray &ray);
    };
}


