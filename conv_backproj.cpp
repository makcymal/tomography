#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
using namespace std;

#define CONFIG_FILE "config.env"
#define GRID_FILE "grid.dat"
#define BACKPROJ_FILE "backproj.dat"
#define PI 3.14158263
typedef vector<vector<double>> Matrix;


class Config {
public:
    int pts, dirs, nx, ny;
    Config();
};


void convolution(Matrix &, Matrix &, Config &);
void backprojection(double &, double &, double &, Matrix &, Config &);


int main() {
    auto config = Config();
    Matrix grid(2 * config.pts + 1, vector<double>(config.dirs)),
           conv(2 * config.pts + 1, vector<double>(config.dirs));

    ifstream grid_buff;
    grid_buff.open(GRID_FILE);

    if (grid_buff.is_open()) {
        for (auto pt = 0; pt < 2 * config.pts + 1; ++pt) {
            for (auto dir = 0; dir < config.dirs; ++dir) {
                grid_buff >> grid[pt][dir];
            }
        }
    }
    grid_buff.close();

    convolution(grid, conv, config);

    double backproj = 0, max = 0;
    ofstream backproj_buff;
    backproj_buff.open(BACKPROJ_FILE);

    for (auto i = -config.nx; i <= config.nx; ++i) {
        double y = -(double)i / config.nx;
        for (auto j = -config.ny; j <= config.ny; ++j) {
            double x = (double)j / config.ny;

            if (x*x + y*y >= 1) backproj = 0;
            else backprojection(backproj, x, y, conv, config);

            if (backproj <= 0) backproj = 0;
            else if (max < backproj) max = backproj;

            backproj_buff << backproj << ' ';
        }
        backproj_buff << '\n';
    }
    backproj_buff << max;
    backproj_buff.close();
}


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


void convolution(Matrix &grid, Matrix &conv, Config &config) {
    double sum;

    for (auto pt = 0; pt < 2 * config.pts + 1; ++pt) {
        for (auto dir = 0; dir < config.dirs; ++dir) {
            sum = 0;
            for (auto pti = 0; pti < 2 * config.pts + 1; ++pti) {
                sum += grid[pti][dir] / (1 - 4 * (pt - pti) * (pt - pti));
            }
            conv[pt][dir] = sum;
        }
    }
}

void backprojection(double &backproj, double &x, double &y, Matrix &conv, Config &config) {
    double sum = 0;

    for (auto dir = 0; dir < config.dirs; ++dir) {
        double phi = PI * (dir - 1) / config.dirs;
        double s = x * cos(phi) + y * sin(phi);
        double rs = s * config.pts;
        int is = floor(s * config.pts);
        double s_mantissa = abs(rs - is);
        sum += (1 - s_mantissa) * conv[is][dir] + s_mantissa * conv[is + 1][dir];
    }

    backproj = 2 * config.pts * sum / (PI * config.dirs);
}
