#include <fstream>
#include <array>
#include <cmath>
#include <cassert>

using namespace std;

#include "util.h"
#include "glob.h"


// Конструктор класса конфигурационной информации
Config::Config() {
    ifstream config_buff;
    string line;

    config_buff.open(CONFIG_FILE);
    if (config_buff.is_open()) {
        while (getline(config_buff, line)) {
            size_t eq_pos = line.find('=');
            string key = line.substr(0, eq_pos);
            int value = stoi(line.substr(eq_pos + 1, line.length() - eq_pos));

            if (key == "POINTS") pts = value;
            else if (key == "DIRECTIONS") dirs = value;
            else if (key == "NX") nx = value;
            else if (key == "NY") ny = value;
        }
    }
    config_buff.close();
}

// Решение квадратного уравнения - возвращает массив,
// заполненный сначала его корнями в порядке убывания, затем значениями NOFLT
array<float, 2> quadeq(float A, float B, float C) {
    array<float, 2> roots = {NOFLT, NOFLT};

    if FLT_EQ(A, 0) {
        assert(!FLT_EQ(B, 0));
        roots[0] = -C / B;
        return roots;
    }

    float D = B * B - 4 * A * C;
    if FLT_EQ(D, 0) {
        roots[0] = -B / 2 / A;
        return roots;
    }

    D = sqrt(D);
    roots[0] = (-B - D) / 2 / A;
    roots[1] = (-B + D) / 2 / A;

    if (roots[0] < roots[1]) {
        swap(roots[0], roots[1]);
    }

    return roots;
}
