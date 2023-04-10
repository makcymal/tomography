#include <iostream>
#include <cmath>
#include <vector>

using namespace std;


#define PI 3.1415926535897932
#define EPSILON std::numeric_limits<double>::epsilon()


// Точка на координатной плоскости.
class Pnt {
public:
    double x, y;

    Pnt(double, double);

    Pnt operator+(const Pnt &) const;

    Pnt operator-(const Pnt &) const;

    Pnt operator*(double) const;

    void dbg() const;
};

// Вектор из начала координат на плоскости.
typedef Pnt Vec;

// Луч из некоторой точки на координатной плоскости.
// Задается начальной точкой, углом поворота против ЧС в [0, 2pi).
class Ray {
public:
    Pnt inception;
    double direction;
    double cd, sd;

    Ray(Pnt, double);
};

// Произвольный эллипс на координатной плоскости.
// Задается точкой центра, длинами большой и малой полуоси, углом поворота против ЧС вокруг центра в [0, 2pi).
class Ellipse {
    Pnt center;
    double a, b, rotation;
    double as, bs, cr, sr;
public:
    Ellipse(Pnt, double, double, double);

    void formula() const;

    [[nodiscard]] bool contains(Pnt &) const;

    [[nodiscard]] vector<Pnt> collide(Ray &ray) const;
};

// Произвольный треугольник на координатной плоскости.
// Задается точками вершин.
class Polygon {
    Pnt vertices[3];
public:
    explicit Polygon(Pnt *);

    [[nodiscard]] bool contains(Pnt &) const;
};

int main() {
    Ellipse ellipse = {Pnt{5, 0}, 3, 2, PI / 2};
    ellipse.formula();
    Ray ray = {Pnt{3.005, 3}, 1.5 * PI};
    auto collisions = ellipse.collide(ray);
    for (auto pnt: collisions) {
        pnt.dbg();
    }
}


Pnt::Pnt(double x, double y) {
    this->x = x, this->y = y;
}

Pnt Pnt::operator+(const Pnt &rhs) const {
    return {x + rhs.x, y + rhs.y};
}

Pnt Pnt::operator-(const Pnt &rhs) const {
    return {x - rhs.x, y - rhs.y};
}

Pnt Pnt::operator*(double rhs) const {
    return {x * rhs, y * rhs};
}

void Pnt::dbg() const {
    cout << "Pnt ( x, y ) = ( " << x << ", " << y << " )\n";
}


Ray::Ray(Pnt inception, double direction) : inception(inception) {
    this->direction = direction;
    cd = cos(direction);
    sd = sin(direction);
}


Ellipse::Ellipse(Pnt center, double a, double b, double rotation) : center(center) {
    this->a = a;
    this->b = b;
    this->rotation = rotation;
    as = a * a;
    bs = b * b;
    cr = cos(rotation);
    sr = sin(rotation);
}

void Ellipse::formula() const {
    cout << bs << "(" << cr << "(x-" << center.x << ") + " << sr << "(y-" << center.y << "))^2 + "
         << as << "(" << sr << "(x-" << center.x << ") - " << cr << "(y-" << center.y << "))^2"
         << " = " << as * bs << endl;
}

[[nodiscard]]
bool Ellipse::contains(Pnt &pnt) const {
    double xi = cr * (pnt.x - center.x) + sr * (pnt.y - center.y),
            nu = sr * (pnt.x - center.x) - cr * (pnt.y - center.y);
    return bs * xi * xi + as * nu * nu <= as * bs;
}

[[nodiscard]]
vector<Pnt> Ellipse::collide(Ray &ray) const {
#define cd ray.cd
#define sd ray.sd

    double dx = ray.inception.x - center.x,
            dy = ray.inception.y - center.y;
    double p = bs * cr * cr + as * sr * sr,
            q = bs * sr * sr + as * cr * cr,
            g = bs - as;

    // Коэффициенты квадратного уравнения относительно расстояния между началом луча и точкой пересечения с эллипсом.
    double A = cd * cd * p + 2 * cr * sr * cd * sd * g + sd * sd * q;
    double B = 2 * dx * cd * p + 2 * cr * sr * g * (dx * sd + dy * cd) + 2 * dy * sd * q;
    double C = dx * dx * p + 2 * dx * dy * cr * sr * g + dy * dy * q - as * bs;

    auto collisions = vector<Pnt>();

    // если A == 0 с поправкой на погрешности
    if (-EPSILON < A && A < EPSILON) {
        cout << "unusual ellipse against ray collision:\n";
        this->formula();
        cout << "ray: x: " << ray.inception.x <<
             " y: " << ray.inception.y <<
             " direction: " << ray.direction << endl;

        // если B == 0 с поправкой на погрешности
        if (-EPSILON < B && B < EPSILON) {
            return collisions;
        }

        double l1 = -C / B;
        collisions.push_back(ray.inception + Pnt(cd, sd) * l1);
        return collisions;
    }

    // Дискриминант
    double D = B * B - 4 * A * C;

    // Пересечений нет
    if (D < 0) return collisions;

    D = sqrt(D);
    double l1 = (-B - D) / 2 / A, l2 = (-B + D) / 2 / A;
    if (l1 >= 0) collisions.push_back(ray.inception + Pnt(cd, sd) * l1);
    if (l2 >= 0) collisions.push_back(ray.inception + Pnt(cd, sd) * l2);
    return collisions;

#undef cd
#undef sd
}


Polygon::Polygon(Pnt vertices[3]) : vertices{vertices[0], vertices[1], vertices[2]} {}

[[nodiscard]]
bool Polygon::contains(Pnt &pnt) const {
    Vec v01 = vertices[1] - vertices[0];
    Vec v02 = vertices[2] - vertices[0];
    Vec v0p = pnt - vertices[0];

    if ((v01.x * v0p.y - v0p.x * v01.y) * (v02.x * v0p.y - v0p.x * v02.y) > 0) {
        return false;
    }

    Vec v10 = vertices[0] - vertices[1];
    Vec v12 = vertices[2] - vertices[1];
    Vec v1p = pnt - vertices[1];

    return (v10.x * v1p.y - v1p.x * v10.y) * (v12.x * v1p.y - v1p.x * v12.y) <= 0;
}
