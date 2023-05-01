#pragma once

#include <vector>
#include <list>
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
DynList quadeq(real A, real B, real C);


void make_jpg(char const *filename, int height, int width, const void *data);

void make_jpg(char const *filename, DynMatr &data);
