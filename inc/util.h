#pragma once

#include <vector>
#include <string>
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
Vector quadeq(real A, real B, real C);


void make_jpg_dat(string &filename, Matrix &data);
