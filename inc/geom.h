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
        float x, y;

        // Конструктор точки
        Pnt(float, float);

        // Квадрат расстояния до точки из начала координат
        [[nodiscard]] float sqrad() const;

        // Проверка на действительность
        [[nodiscard]] bool is_nopnt() const;
    };

    // Недействительная точка
    const Pnt NOPNT = Pnt(NOFLT, NOFLT);


    // Единичный вектор, задается зенитным в [0, 2pi) углом
    class Vec {
    public:
        // Косинус и синус зенитного угла
        float zencos, zensin;

        // Конструктор вектора по зенитному углу
        explicit Vec(float);

        // Конструктор вектора по значениям косинуса и синуса
        Vec(float, float);

        // Вектор, перпендикулярный данному
        [[nodiscard]] Vec perp() const;

        // Направляющие косинусы
        [[nodiscard]] array<float, 2> guiding_cos() const;

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
    // Задается точкой центра, длинами большой и малой полуоси, зенитным углом поворота
    class Ellipse {
        // Центр эллипса
        Pnt center;
        // Квадраты длин большой и малой полуосей
        float sqa, sqb;
        // Косинусы и синусы угла поворота
        float rotcos, rotsin;
    public:
        // Лямбда, задающая значение коэффициента ослабления внутри эллипса
        function<float(const Pnt &)> attenuation;

        // Конструктор эллипса
        Ellipse(Pnt, float, float, float, function<float(const Pnt &)>);

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
        function<float(const Pnt &)> attenuation;

        // Конструктор треугольника
        Polygon(array<Pnt, 3>, function<float(const Pnt &)>);

        // Проверка на вхождение точки
        [[nodiscard]] bool contains(Pnt &) const;

        // Точки пересечения с заданным лучом
        [[nodiscard]] array<Pnt, 2> collide(Ray &ray) const;
    };


    // Область - круг радиуса 1 с центром в начале координат, представляющий собой сечение области G
    class Area {
        // Содержащиеся внутри треугольники
        std::vector<Polygon> polygons;
        // Содержащиеся внутри эллипсы
        std::vector<Ellipse> ellipses;
    public:
        // Лямбда, задающая значение коэффициента ослабления внутри эллипса
        std::function<float(const Pnt &)> attenuation;

        // Конструктор области
        Area(std::vector<Polygon>, std::vector<Ellipse>, std::function<float(const Pnt &)>);

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
        float x, y, z;

        // Конструктор точки
        Pnt(float, float, float);

        // Квадрат расстояния до точки из начала координат
        [[nodiscard]] float sqrad() const;

        // Проверка на действительность
        [[nodiscard]] bool is_nopnt() const;
    };

    // Недействительная точка
    const Pnt NOPNT = Pnt(NOFLT, NOFLT, NOFLT);


    // Единичный вектор, задается зенитным в [0, 2pi) и азимутальным в [0, pi] углом
    class Vec {
    public:
        // Косинусы и синусы зенитного и азимутального углов
        float zencos, zensin, azicos, azisin;

        // Конструктор вектора по зенитному и азимутальному углу
        Vec(float, float);

        // Конструктор вектора по значениям косинусов и синусов
        Vec(float, float, float, float);

        // Вектор, перпендикулярный данному
        [[nodiscard]] Vec perp() const;

        // Направляющие косинусы
        [[nodiscard]] array<float, 3> guiding_cos() const;

        // Проверка на действительность
        [[nodiscard]] bool is_novec() const;
    };

    // Недействительный вектор
    const Vec NOVEC = Vec(0, 0, 0, 0);


    // Луч как закрепленный вектор
    class Ray {
    public:// Точка начала луча
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
