#pragma once

#include <cmath>
#include <ostream>

#define EPS (double)1e-9
#define M_EPS (double)1e-12
#define INF (double)1e+9
#define M_INF (double)1e+12

#define ZERO Point3(0.0)

#define NULV Vector3(0.0)
#define UP Vector3(0.0, 0.0, 1.0)
#define RIGHT Vector3(0.0, 1.0, 0.0)
#define FRONT Vector3(1.0, 0.0, 0.0)

#define ID Matrix33(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0)

namespace RayTracer {
class Pixel {
public:
    unsigned x;
    unsigned y;

    Pixel() = default;
    Pixel(unsigned x, unsigned y)
        : x(x)
        , y(y) { };

public:
    friend std::ostream& operator<<(std::ostream& out, Pixel& p)
    {
        return out << "[" << p.x << "," << p.y << "]";
    }
};

class Vector3 {
public:
    double x;
    double y;
    double z;

    Vector3()
        : x(0)
        , y(0)
        , z(0) { };
    Vector3(double v)
        : x(v)
        , y(v)
        , z(v) { };
    Vector3(double x, double y, double z)
        : x(x)
        , y(y)
        , z(z) { };

public:
    Vector3 operator*(const double& l) const
    {
        return Vector3(x * l, y * l, z * l);
    }

    void operator*=(const double& l)
    {
        x *= l;
        y *= l;
        z *= l;
    }

    Vector3 operator/(const double& l) const
    {
        return Vector3(x / l, y / l, z / l);
    }

    void operator/=(const double& l)
    {
        x /= l;
        y /= l;
        z /= l;
    }

    Vector3 operator+(const Vector3& v) const
    {
        return Vector3(x + v.x, y + v.y, z + v.z);
    }

    void operator+=(const Vector3& v)
    {
        x += v.x;
        y += v.y;
        z += v.z;
    }

    Vector3 operator-(const Vector3& v) const
    {
        return Vector3(x - v.x, y - v.y, z - v.z);
    }

    void operator-=(const Vector3& v)
    {
        x -= v.x;
        y -= v.y;
        z -= v.z;
    }

    Vector3 operator/(const Vector3& v) const
    {
        return Vector3(x / v.x, y / v.y, z / v.z);
    }

    void operator/=(const Vector3& v)
    {
        x /= v.x;
        y /= v.y;
        z /= v.z;
    }

    Vector3 operator-() const
    {
        return Vector3(-x, -y, -z);
    }

    /* Dot product */
    double operator*(const Vector3& v) const
    {
        return x * v.x + y * v.y + z * v.z;
    }

    void operator*=(const Vector3& v)
    {
        x *= v.x;
        y *= v.y;
        z *= v.z;
    }

    /* Cross product */
    Vector3 operator^(const Vector3& v) const
    {
        return Vector3(
            y * v.z - z * v.y,
            z * v.x - x * v.z,
            x * v.y - y * v.x);
    }

    void operator^=(const Vector3& v)
    {
        double x_ = y * v.z - z * v.y;
        double y_ = z * v.x - x * v.z;
        double z_ = x * v.y - y * v.x;

        x = x_;
        y = y_;
        z = z_;
    }

    double norm_squared() const
    {
        return x * x + y * y + z * z;
    }

    double norm() const
    {
        return sqrt(norm_squared());
    }

    Vector3 normalize() const
    {
        double n = this->norm();
        return Vector3(x / n, y / n, z / n);
    }

    friend std::ostream& operator<<(std::ostream& out, Vector3& v)
    {
        return out << "V(" << v.x << "," << v.y << "," << v.z << ")";
    }
};

class Point2 {
public:
    double x;
    double y;

    Point2() = default;
    Point2(double v)
        : x(v)
        , y(v) { };
    Point2(double x, double y)
        : x(x)
        , y(y) { };

public:
    Point2 operator*(const double& l) const
    {
        return Point2(x * l, y * l);
    }

    void operator*=(const double& l)
    {
        x *= l;
        y *= l;
    }

    Point2 operator/(const double& l) const
    {
        return Point2(x / l, y / l);
    }

    void operator/=(const double& l)
    {
        x /= l;
        y /= l;
    }

    Point2 operator+(const double& l) const
    {
        return Point2(x + l, y + l);
    }

    void operator+=(const double& l)
    {
        x += l;
        y += l;
    }

    Point2 operator-(const double& l) const
    {
        return Point2(x - l, y - l);
    }

    void operator-=(const double& l)
    {
        x -= l;
        y -= l;
    }

    Point2 operator+(const Vector3& v) const
    {
        return Point2(x + v.x, y + v.y);
    }

    void operator+=(const Vector3& v)
    {
        x += v.x;
        y += v.y;
    }

    Point2 operator-(const Vector3& v) const
    {
        return Point2(x - v.x, y - v.y);
    }

    void operator-=(const Vector3& v)
    {
        x -= v.x;
        y -= v.y;
    }

    Point2 operator+(const Point2& p) const
    {
        return Point2(x + p.x, y + p.y);
    }

    void operator+=(const Point2& p)
    {
        x += p.x;
        y += p.y;
    }

    Point2 operator-(const Point2& p) const
    {
        return Point2(x - p.x, y - p.y);
    }

    void operator-=(const Point2& p)
    {
        x -= p.x;
        y -= p.y;
    }

    Point2 operator-() const
    {
        return Point2(-x, -y);
    }

    friend std::ostream& operator<<(std::ostream& out, Point2& p)
    {
        return out << "P(" << p.x << "," << p.y << ")";
    }
};

class Point3 {
public:
    double x;
    double y;
    double z;

    Point3()
        : x(0)
        , y(0)
        , z(0) { };
    Point3(double v)
        : x(v)
        , y(v)
        , z(v) { };
    Point3(double x, double y, double z)
        : x(x)
        , y(y)
        , z(z) { };

public:
    Point3 operator*(const double& l) const
    {
        return Point3(x * l, y * l, z * l);
    }

    void operator*=(const double& l)
    {
        x *= l;
        y *= l;
        z *= l;
    }

    Point3 operator/(const double& l) const
    {
        return Point3(x / l, y / l, z / l);
    }

    void operator/=(const double& l)
    {
        x /= l;
        y /= l;
        z /= l;
    }

    Point3 operator+(const double& l) const
    {
        return Point3(x + l, y + l, z + l);
    }

    void operator+=(const double& l)
    {
        x += l;
        y += l;
        z += l;
    }

    Point3 operator-(const double& l) const
    {
        return Point3(x - l, y - l, z - l);
    }

    void operator-=(const double& l)
    {
        x -= l;
        y -= l;
        z -= l;
    }

    Point3 operator+(const Vector3& v) const
    {
        return Point3(x + v.x, y + v.y, z + v.z);
    }

    void operator+=(const Vector3& v)
    {
        x += v.x;
        y += v.y;
        z += v.z;
    }

    Point3 operator-(const Vector3& v) const
    {
        return Point3(x - v.x, y - v.y, z - v.z);
    }

    void operator-=(const Vector3& v)
    {
        x -= v.x;
        y -= v.y;
        z -= v.z;
    }

    Point3 operator+(const Point3& p) const
    {
        return Point3(x + p.x, y + p.y, z + p.z);
    }

    void operator+=(const Point3& p)
    {
        x += p.x;
        y += p.y;
        z += p.z;
    }

    Vector3 operator-(const Point3& p) const
    {
        return Vector3(x - p.x, y - p.y, z - p.z);
    }

    void operator-=(const Point3& p)
    {
        x -= p.x;
        y -= p.y;
        z -= p.z;
    }

    Point3 operator-() const
    {
        return Point3(-x, -y, -z);
    }

    friend std::ostream& operator<<(std::ostream& out, Point3& p)
    {
        return out << "P(" << p.x << "," << p.y << "," << p.z << ")";
    }
};

class Matrix33 {
public:
    std::array<std::array<double, 3>, 3> matrix;

    Matrix33() = default;
    Matrix33(const Vector3& c1, const Vector3& c2, const Vector3& c3)
    {
        matrix[0][0] = c1.x;
        matrix[1][0] = c1.y;
        matrix[2][0] = c1.z;
        matrix[0][1] = c2.x;
        matrix[1][1] = c2.y;
        matrix[2][1] = c2.z;
        matrix[0][2] = c3.x;
        matrix[1][2] = c3.y;
        matrix[2][2] = c3.z;
    };
    Matrix33(
        double a, double b, double c,
        double d, double e, double f,
        double g, double h, double i)
    {
        matrix[0][0] = a;
        matrix[0][1] = b;
        matrix[0][2] = c;
        matrix[1][0] = d;
        matrix[1][1] = e;
        matrix[1][2] = f;
        matrix[2][0] = g;
        matrix[2][1] = h;
        matrix[2][2] = i;
    };

public:
    Matrix33 operator*(const double& l) const
    {
        return Matrix33(
            matrix[0][0] * l, matrix[0][1] * l, matrix[0][2] * l,
            matrix[1][0] * l, matrix[1][1] * l, matrix[1][2] * l,
            matrix[2][0] * l, matrix[2][1] * l, matrix[2][2] * l);
    }

    void operator*=(const double& l)
    {
        matrix[0][0] *= l;
        matrix[0][1] *= l;
        matrix[0][2] *= l;
        matrix[1][0] *= l;
        matrix[1][1] *= l;
        matrix[1][2] *= l;
        matrix[2][0] *= l;
        matrix[2][1] *= l;
        matrix[2][2] *= l;
    }

    Matrix33 operator/(const double& l) const
    {
        return Matrix33(
            matrix[0][0] / l, matrix[0][1] / l, matrix[0][2] / l,
            matrix[1][0] / l, matrix[1][1] / l, matrix[1][2] / l,
            matrix[2][0] / l, matrix[2][1] / l, matrix[2][2] / l);
    }

    void operator/=(const double& l)
    {
        matrix[0][0] /= l;
        matrix[0][1] /= l;
        matrix[0][2] /= l;
        matrix[1][0] /= l;
        matrix[1][1] /= l;
        matrix[1][2] /= l;
        matrix[2][0] /= l;
        matrix[2][1] /= l;
        matrix[2][2] /= l;
    }

    Matrix33 operator+(const Matrix33& m) const
    {
        return Matrix33(
            matrix[0][0] + m.matrix[0][0], matrix[0][1] + m.matrix[0][1], matrix[0][2] + m.matrix[0][2],
            matrix[1][0] + m.matrix[1][0], matrix[1][1] + m.matrix[1][1], matrix[1][2] + m.matrix[1][2],
            matrix[2][0] + m.matrix[2][0], matrix[2][1] + m.matrix[2][1], matrix[2][2] + m.matrix[2][2]);
    }

    void operator+=(const Matrix33& m)
    {
        matrix[0][0] += m.matrix[0][0];
        matrix[0][1] += m.matrix[0][1];
        matrix[0][2] += m.matrix[0][2];
        matrix[1][0] += m.matrix[1][0];
        matrix[1][1] += m.matrix[1][1];
        matrix[1][2] += m.matrix[1][2];
        matrix[2][0] += m.matrix[2][0];
        matrix[2][1] += m.matrix[2][1];
        matrix[2][2] += m.matrix[2][2];
    }

    Matrix33 operator-(const Matrix33& m) const
    {
        return Matrix33(
            matrix[0][0] - m.matrix[0][0], matrix[0][1] - m.matrix[0][1], matrix[0][2] - m.matrix[0][2],
            matrix[1][0] - m.matrix[1][0], matrix[1][1] - m.matrix[1][1], matrix[1][2] - m.matrix[1][2],
            matrix[2][0] - m.matrix[2][0], matrix[2][1] - m.matrix[2][1], matrix[2][2] - m.matrix[2][2]);
    }

    void operator-=(const Matrix33& m)
    {
        matrix[0][0] -= m.matrix[0][0];
        matrix[0][1] -= m.matrix[0][1];
        matrix[0][2] -= m.matrix[0][2];
        matrix[1][0] -= m.matrix[1][0];
        matrix[1][1] -= m.matrix[1][1];
        matrix[1][2] -= m.matrix[1][2];
        matrix[2][0] -= m.matrix[2][0];
        matrix[2][1] -= m.matrix[2][1];
        matrix[2][2] -= m.matrix[2][2];
    }

    Matrix33 operator*(const Matrix33& m) const
    {
        return Matrix33(
            matrix[0][0] * m.matrix[0][0] + matrix[0][1] * m.matrix[1][0] + matrix[0][2] * m.matrix[2][0],
            matrix[0][0] * m.matrix[0][1] + matrix[0][1] * m.matrix[1][1] + matrix[0][2] * m.matrix[2][1],
            matrix[0][0] * m.matrix[0][2] + matrix[0][1] * m.matrix[1][2] + matrix[0][2] * m.matrix[2][2],
            matrix[1][0] * m.matrix[0][0] + matrix[1][1] * m.matrix[1][0] + matrix[1][2] * m.matrix[2][0],
            matrix[1][0] * m.matrix[0][1] + matrix[1][1] * m.matrix[1][1] + matrix[1][2] * m.matrix[2][1],
            matrix[1][0] * m.matrix[0][2] + matrix[1][1] * m.matrix[1][2] + matrix[1][2] * m.matrix[2][2],
            matrix[2][0] * m.matrix[0][0] + matrix[2][1] * m.matrix[1][0] + matrix[2][2] * m.matrix[2][0],
            matrix[2][0] * m.matrix[0][1] + matrix[2][1] * m.matrix[1][1] + matrix[2][2] * m.matrix[2][1],
            matrix[2][0] * m.matrix[0][2] + matrix[2][1] * m.matrix[1][2] + matrix[2][2] * m.matrix[2][2]);
    }

    void operator*=(const Matrix33& m)
    {
        double a = matrix[0][0] * m.matrix[0][0] + matrix[0][1] * m.matrix[1][0] + matrix[0][2] * m.matrix[2][0];
        double b = matrix[0][0] * m.matrix[0][1] + matrix[0][1] * m.matrix[1][1] + matrix[0][2] * m.matrix[2][1];
        double c = matrix[0][0] * m.matrix[0][2] + matrix[0][1] * m.matrix[1][2] + matrix[0][2] * m.matrix[2][2];
        double d = matrix[1][0] * m.matrix[0][0] + matrix[1][1] * m.matrix[1][0] + matrix[1][2] * m.matrix[2][0];
        double e = matrix[1][0] * m.matrix[0][1] + matrix[1][1] * m.matrix[1][1] + matrix[1][2] * m.matrix[2][1];
        double f = matrix[1][0] * m.matrix[0][2] + matrix[1][1] * m.matrix[1][2] + matrix[1][2] * m.matrix[2][2];
        double g = matrix[2][0] * m.matrix[0][0] + matrix[2][1] * m.matrix[1][0] + matrix[2][2] * m.matrix[2][0];
        double h = matrix[2][0] * m.matrix[0][1] + matrix[2][1] * m.matrix[1][1] + matrix[2][2] * m.matrix[2][1];
        double i = matrix[2][0] * m.matrix[0][2] + matrix[2][1] * m.matrix[1][2] + matrix[2][2] * m.matrix[2][2];

        matrix[0][0] = a;
        matrix[0][1] = b;
        matrix[0][2] = c;
        matrix[1][0] = d;
        matrix[1][1] = e;
        matrix[1][2] = f;
        matrix[2][0] = g;
        matrix[2][1] = h;
        matrix[2][2] = i;
    }

    Vector3 operator*(const Vector3& v) const
    {
        return Vector3(
            matrix[0][0] * v.x + matrix[0][1] * v.y + matrix[0][2] * v.z,
            matrix[1][0] * v.x + matrix[1][1] * v.y + matrix[1][2] * v.z,
            matrix[2][0] * v.x + matrix[2][1] * v.y + matrix[2][2] * v.z);
    }

    Point3 operator*(const Point3& p) const
    {
        return Point3(
            matrix[0][0] * p.x + matrix[0][1] * p.y + matrix[0][2] * p.z,
            matrix[1][0] * p.x + matrix[1][1] * p.y + matrix[1][2] * p.z,
            matrix[2][0] * p.x + matrix[2][1] * p.y + matrix[2][2] * p.z);
    }

    double operator()(int i, int j) const
    {
        return matrix[i][j];
    }

    Matrix33 inverse() const
    {
        double det = determinant();
        double invdet = 1 / det;

        return Matrix33(
            (matrix[1][1] * matrix[2][2] - matrix[1][2] * matrix[2][1]) * invdet,
            (matrix[0][2] * matrix[2][1] - matrix[0][1] * matrix[2][2]) * invdet,
            (matrix[0][1] * matrix[1][2] - matrix[0][2] * matrix[1][1]) * invdet,
            (matrix[1][2] * matrix[2][0] - matrix[1][0] * matrix[2][2]) * invdet,
            (matrix[0][0] * matrix[2][2] - matrix[0][2] * matrix[2][0]) * invdet,
            (matrix[1][0] * matrix[0][2] - matrix[0][0] * matrix[1][2]) * invdet,
            (matrix[1][0] * matrix[2][1] - matrix[2][0] * matrix[1][1]) * invdet,
            (matrix[2][0] * matrix[0][1] - matrix[0][0] * matrix[2][1]) * invdet,
            (matrix[0][0] * matrix[1][1] - matrix[1][0] * matrix[0][1]) * invdet);
    }

    double determinant() const
    {
        return matrix[0][0] * (matrix[1][1] * matrix[2][2] - matrix[1][2] * matrix[2][1]) - matrix[0][1] * (matrix[1][0] * matrix[2][2] - matrix[1][2] * matrix[2][0]) + matrix[0][2] * (matrix[1][0] * matrix[2][1] - matrix[1][1] * matrix[2][0]);
    }

    friend std::ostream& operator<<(std::ostream& out, Matrix33& m)
    {
        return out << "M(" << m.matrix[0][0] << "," << m.matrix[0][1] << "," << m.matrix[0][2] << ";"
                   << m.matrix[1][0] << "," << m.matrix[1][1] << "," << m.matrix[1][2] << ";"
                   << m.matrix[2][0] << "," << m.matrix[2][1] << "," << m.matrix[2][2] << ")";
    }
};

class Ray {
public:
    Point3 origin;
    Vector3 direction;

    Ray() = default;
    Ray(const Point3& o, const Vector3& d)
        : origin(o)
        , direction(d.normalize()) { };
};

/* Helper constructors */
Matrix33 RotXMatrix(double a)
{
    return Matrix33(
        1.0, 0.0, 0.0,
        0.0, cos(a), -sin(a),
        0.0, sin(a), cos(a));
}

Matrix33 RotYMatrix(double a)
{
    return Matrix33(
        cos(a), 0.0, sin(a),
        0.0, 1.0, 0.0,
        -sin(a), 0.0, cos(a));
}

Matrix33 RotZMatrix(double a)
{
    return Matrix33(
        cos(a), -sin(a), 0.0,
        sin(a), cos(a), 0.0,
        0.0, 0.0, 1.0);
}
}
