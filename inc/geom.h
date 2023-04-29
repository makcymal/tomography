#pragma once

#include <array>
#include <functional>
#include "glob.h"

using namespace std;

namespace D2 {
    // Точка на координатной плоскости
    class Pnt {
    public:
        // Координаты точки
        real x, y;

        // Конструктор точки
        Pnt(real, real);

        // Квадрат расстояния до точки из начала координат
        [[nodiscard]] real sqrad() const;

        // Проверка на действительность
        [[nodiscard]] bool is_nopnt() const;
    };

    // Недействительная точка
    const Pnt NOPNT = Pnt(NOREAL, NOREAL);


    // Единичный вектор, задается азимутальным в [0, 2pi) углом
    class Vec {
    public:
        // Косинус и синус азимутального угла
        real azicos, azisin;

        // Конструктор вектора по азимутальному углу
        explicit Vec(real);

        // Конструктор вектора по значениям косинуса и синуса
        Vec(real, real);

        // Вектор, перпендикулярный данному
        [[nodiscard]] Vec perp() const;

        // Направляющие косинусы
        [[nodiscard]] List(2) guiding_cos() const;

        // Проверка на действительность
        [[nodiscard]] bool is_novec() const;
    };

    // Недействительный вектор
    const Vec NOVEC = Vec(0, 0);


    // Луч как закрепленный вектор
    class Ray {
    public:
        // Точка начала луча
        Pnt inception;
        // Направляющий вектор луча
        Vec direction;

        // Конструктор луча
        Ray(Pnt, Vec);

        // Луч, перпендикулярный данному
        [[nodiscard]] Ray perp() const;

        // Проверка на действительность
        [[nodiscard]] bool is_noray() const;
    };

    // Недействительный луч
    const Ray NORAY = Ray(NOPNT, NOVEC);


    // Произвольный эллипс на координатной плоскости
    // Задается точкой центра, длинами большой и малой полуоси, азимутальным углом поворота
    class Ellipse {
        // Центр эллипса
        Pnt center;
        // Квадраты длин большой и малой полуосей
        real sqa, sqb;
        // Косинусы и синусы угла поворота
        real rotcos, rotsin;
    public:
        // Лямбда, задающая значение коэффициента ослабления внутри эллипса
        function<real(const Pnt &)> attenuation;

        // Конструктор эллипса
        Ellipse(Pnt, real, real, real, function<real(const Pnt &)>);

        // Формула для графопостроителя
        void formula() const;

        // Проверка на вхождение точки
        [[nodiscard]] bool contains(Pnt &) const;

        // Точки пересечения с заданным лучом
        [[nodiscard]] array<Pnt, 2> collide(Ray &ray) const;
    };


    // Произвольный треугольник на координатной плоскости, задается точками вершин
    class Polygon {
        // Точки вершин, в порядке по часовой стрелке
        array<Pnt, 3> vertices;
    public:
        // Лямбда, задающая значение коэффициента ослабления внутри треугольника
        function<real(const Pnt &)> attenuation;

        // Конструктор треугольника
        Polygon(array<Pnt, 3>, function<real(const Pnt &)>);

        // Проверка на вхождение точки
        [[nodiscard]] bool contains(Pnt &) const;

        // Точки пересечения с заданным лучом
        [[nodiscard]] array<Pnt, 2> collide(Ray &ray) const;
    };


    // Область - круг радиуса 1 с центром в начале координат, представляющий собой сечение области G
    class Area {
        // Содержащиеся внутри треугольники
        vector<Polygon> polygons;
        // Содержащиеся внутри эллипсы
        vector<Ellipse> ellipses;
    public:
        // Лямбда, задающая значение коэффициента ослабления внутри эллипса
        function<real(const Pnt &)> attenuation;

        // Конструктор области
        Area(vector<Polygon>, vector<Ellipse>, function<real(const Pnt &)>);

        // Создание изображения "area.jpg" с полутоновым изображением области
        void image(int);
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
        Pnt(real, real, real);

        // Квадрат расстояния до точки из начала координат
        [[nodiscard]] real sqrad() const;

        // Проверка на действительность
        [[nodiscard]] bool is_nopnt() const;
    };

    // Недействительная точка
    const Pnt NOPNT = Pnt(NOREAL, NOREAL, NOREAL);


    // Единичный вектор, задается азимутальным в [0, 2pi) и зенитным в [0, pi] углом
    class Vec {
    public:
        // Косинусы и синусы азимутального и зенитного углов
        real azicos, azisin, zencos, zensin;

        // Конструктор вектора по азимутальному и зенитному углу
        Vec(real, real);

        // Конструктор вектора по значениям косинусов и синусов
        Vec(real, real, real, real);

        // Вектор, перпендикулярный данному
        [[nodiscard]] Vec perp() const;

        // Направляющие косинусы
        [[nodiscard]] List(3) guiding_cos() const;

        // Проверка на действительность
        [[nodiscard]] bool is_novec() const;
    };

    // Недействительный вектор
    const Vec NOVEC = Vec(0, 0, 0, 0);


    // Луч как закрепленный вектор
    class Ray {
    public:
        // Точка начала луча
        Pnt inception;
        // Направляющий вектор луча
        Vec direction;

        // Конструктор луча
        Ray(Pnt, Vec);

        // Луч, перпендикулярный данному
        [[nodiscard]] Ray perp() const;

        // Проверка на действительность
        [[nodiscard]] bool is_noray() const;
    };

    // Недействительный луч
    const Ray NORAY = Ray(NOPNT, NOVEC);


    class Sphere {
    public:
        // Точки пересечения с заданным лучом
        static array<Pnt, 2> collide(Ray &ray);
    };
}


