#include <fstream>
#include <array>
#include <cmath>
#include <cassert>
#include "util.h"
#include "glob.h"

using namespace std;



// Конструктор класса конфигурационной информации
Config::Config() {
    ifstream config_buff;
    string line;

    config_buff.open(CONFIG_FILE);
    if (config_buff.is_open()) {
        while (getline(config_buff, line)) {
            // если строка - комментарий (содержит #)
            if (line.find('#') != string::npos) continue;

            size_t eq_pos = line.find('=');
            string key = line.substr(0, eq_pos);
            int value = stoi(line.substr(eq_pos + 1, line.length() - eq_pos));

            if (key == "NRHO") n_rho = value;
            else if (key == "NPHI") n_phi = value;
            else if (key == "NX") n_x = value;
            else if (key == "NY") n_y = value;
        }
    }
    config_buff.close();
}


// Решение квадратного уравнения - возвращает массив,
// заполненный сначала его корнями в порядке убывания, затем значениями NOREAL
DynList quadeq(real A, real B, real C) {
    if real_eq(A, 0) {
        assert(!real_eq(B, 0));
        return {-C / B};
    }

    real D = B * B - 4 * A * C;
    if real_eq(D, 0) {
        return {-B / 2 / A};
    }

    DynList roots;
    D = sqrt(D);
    roots[0] = (-B - D) / 2 / A;
    roots[1] = (-B + D) / 2 / A;
    if (roots[0] < roots[1]) {
        swap(roots[0], roots[1]);
    }
    return roots;
}


#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

void make_jpg(char const *filename, int height, int width, const void *data) {
    stbi_write_jpg(filename, width, height, 1, data, 100);
}

void make_jpg(char const *filename, DynMatr &data) {
    unsigned char ndata[data.size()][data[0].size()];
    real m = 0;
    for (int y = 0; y < data.size(); ++y) {
        for (int x = 0; x < data[0].size(); ++x) {
            m = max(data[y][x], m);
        }
    }
    for (int y = 0; y < data.size(); ++y) {
        for (int x = 0; x < data[0].size(); ++x) {
            ndata[y][x] = 255.f * data[y][x] / m;
        }
    }
    stbi_write_jpg(filename, data[0].size(), data.size(), 1, ndata, 100);
}
