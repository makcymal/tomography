#pragma once

#include <array>
#include "glob.h"

using namespace std;


// Класс конфигурационной информации
class Config {
public:
    int n_rho, n_phi, n_x, n_y;

    Config();
};


// Решение квадратного уравнения - возвращает массив,
// заполненный сначала его корнями в порядке убывания, затем значениями NOREAL
List(2) quadeq(real A, real B, real C);
