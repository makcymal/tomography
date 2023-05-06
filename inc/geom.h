#pragma once

#include <functional>
#include "glob.h"
#include "util.h"

using namespace std;


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


    // Проверка на вхождение точки
    [[nodiscard]] bool contains(Pnt &pnt) const;

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
    explicit Area(function<real(const Pnt &)> atten, vector<Ellipse> ellipse = {}, vector<Polygon> polygon = {});

    // Коэффиент ослабления внутри области с учетом внутренних областей
    real attenuation(Pnt &pnt, bool intersections = false) const;

    Matrix image(Config &config, bool intersections = false) const;
};

// Функции распределения коэффициента ослабления
#define RADIUS_ATTEN(diap) ([](const Pnt &pnt) { return pnt.sqrad() * diap; })
#define INV_RADIUS_ATTEN(diap) ([](const Pnt &pnt) { return ((1 - pnt.sqrad()) * diap); })
#define CATHETS_ATTEN(diap) ([](const Pnt &pnt) { return (std::abs(pnt.x) + std::abs(pnt.y)) * (diap / 2); })
#define OX_ATTEN(diap) ([](const Pnt &pnt) { return std::abs(pnt.x) * diap; })
#define OY_ATTEN(diap) ([](const Pnt &pnt) { return std::abs(pnt.y) * diap; })
#define CONST_ATTEN(val) ([](const Pnt &pnt) { return val; })
