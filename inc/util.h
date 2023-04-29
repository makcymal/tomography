#pragma once

#include <vector>
#include <list>
#include "glob.h"
#include "geom.h"

using namespace std;


// Класс конфигурационной информации
class Config {
public:
    int n_rho, n_phi, n_x, n_y;

    Config();
};


// Решение квадратного уравнения - возвращает массив,
// заполненный сначала его корнями в порядке убывания, затем значениями NOREAL
List quadeq(real A, real B, real C);


void make_jpg(char const *filename, int width, int height, const void *data);
