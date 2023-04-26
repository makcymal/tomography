#pragma once

#include <array>

using namespace std;


// Класс конфигурационной информации
class Config {
public:
    int pts, dirs, nx, ny;

    Config();
};


// Решение квадратного уравнения - возвращает массив,
// заполненный сначала его корнями в порядке убывания, затем значениями NOFLT
array<float, 2> quadeq(float A, float B, float C);
