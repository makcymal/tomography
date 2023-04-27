#pragma GCC optimize("O3")

#include <iostream>
#include <vector>
#include <cmath>

using namespace std;

#include "geom.h"
#include "glob.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "stb_image_write.h"


void convolution(Matrix &grid, Matrix &conv) {
    double sum;

    for (auto pt = 0; pt < 81; ++pt) {
        for (auto dir = 0; dir < 100; ++dir) {
            sum = 0;
            for (auto pti = 0; pti < 81; ++pti) {
                sum += grid[pti][dir] / (1 - 4 * (pt - pti) * (pt - pti));
            }
            conv[pt][dir] = sum;
        }
    }
}


float backprojection(float &x, float &y, Matrix &conv) {
    double sum = 0;

    for (auto dir = 0; dir < 100; ++dir) {
        double phi = PI * dir / 100;
        double ro = x * cos(phi) + y * sin(phi);

        double rs = ro * 40;
        int is = floor(ro * 40);
        double s_mantissa = abs(rs - is);

        sum += (1 - s_mantissa) * conv[40 + is][dir] +
               s_mantissa * conv[40 + is + 1][dir];

        if (abs(ro) > 1) {
            cout << x << ' ' << y << ' ' << dir << ' ' << ro << ' ' << rs << ' ' << is << endl;
        }
    }

    return 80.f * sum / (PI * 100);
}


int main() {
    vector<vector<float>> dat(100, vector<float>(150, 0));

    for (auto pt = 0; pt < 81; ++pt) {
        for (auto dir = 0; dir < 100; ++dir) {
            float ro = -1.f + (float) pt / 40;
            float phi = dir * PI / 100;
            dat[pt][dir] = 2 * sqrt(1 - ro * ro);
        }
    }

    vector<vector<float>> conv(100, vector<float>(150, 0));

    convolution(dat, conv);

    vector<vector<float>> func(350, vector<float>(350, 0));
    unsigned char pixels[350][350] = {0};


    float m = 0;

    for (auto i = -150; i < 151; ++i) {
        float y = -1.f *  ((float)i / 150);
        for (auto j = -150; j < 151; ++j) {
            float x = 1.f *  ((float)j / 150);

            if (x * x + y * y <= 1) {
                func[150 + i][150 + j] = backprojection(x, y, conv);
                m = max(m, func[150 + i][150 + j]);
            } else {
                func[150 + i][150 + j] = 0;
            }
        }
    }

    for (auto i = -150; i < 151; ++i) {
        for (auto j = -150; j < 151; ++j) {
            pixels[150 + i][150 + j] = func[150 + i][150 + j] / m * 255;
            cout << -1.0 * (float)i / 150.0 << ' ' << 1.0 * (float)j / 150.0 << ' ' << func[150 + i][150 + j] << '\n';
        }
        cout << endl;
    }



    stbi_write_jpg("img/func.jpg", 301, 301, 1, &func, 100);

    return 0;
}



