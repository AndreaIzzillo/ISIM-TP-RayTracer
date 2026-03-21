#pragma once

#include <ostream>
#include <cmath>

#define EPS 0.00001
#define INF 10000.0

#define X Vector(1.0, 0.0, 0.0)
#define Y Vector(0.0, 1.0, 0.0)
#define Z Vector(0.0, 0.0, 1.0)

namespace RayTracer
{
    class Pixel
    {
    public:
        unsigned x, y;

        Pixel() = default;
        Pixel(unsigned x, unsigned y): x(x), y(y) {};

    public:
        friend std::ostream& operator<<(std::ostream& out, Pixel& p)
        {
            return out << "[" << p.x << "," << p.y << "]";
        }
    };

    class Vector3
    {
    public:
        double x, y, z;

        Vector3() : x(0), y(0), z(0) {};
        Vector3(double x, double y, double z) : x(x), y(y), z(z) {};

    public:
        Vector3 operator*(const double& l) const
        {
            return Vector3(x * l, y * l, z * l);
        }

        Vector3 operator/(const double& l) const
        {
            return Vector3(x / l, y / l, z / l);
        }

        Vector3 operator+(const Vector3& v) const
        {
            return Vector3(x + v.x, y + v.y, z + v.z);
        }

        Vector3 operator-(const Vector3& v) const
        {
            return Vector3(x - v.x, y - v.y, z - v.z);
        }

        Vector3 operator/(const Vector3& v) const
        {
            return Vector3(x / v.x, y / v.y, z / v.z);
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

        /* Cross product */
        Vector3 operator^(const Vector3& v) const
        {
            return Vector3(
                y * v.z - z * v.y,
                z * v.x - x * v.z,
                x * v.y - y * v.x
            );
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

    class Point3
    {
    public:
        double x, y, z;

        Point3() : x(0), y(0), z(0) {};
        Point3(double x, double y, double z) : x(x), y(y), z(z) {};

    public:
        Point3 operator+(const Vector3& v)
        {
            return Point3(x + v.x, y + v.y, z + v.z);
        }

        Point3 operator-(const Vector3& v)
        {
            return Point3(x - v.x, y - v.y, z - v.z);
        }

        Point3 operator+(const Point3& p)
        {
            return Point3(x + p.x, y + p.x, z + p.z);
        }

        Vector3 operator-(const Point3& p)
        {
            return Vector3(x - p.x, y - p.y, z - p.z);
        }

        Point3 operator-()
        {
            return Point3(-x, -y, -z);
        }

        friend std::ostream& operator<<(std::ostream& out, Point3& p)
        {
            return out << "P(" << p.x << "," << p.y << "," << p.z << ")";
        }
    };

    class Ray
    {
    public:
        Point3 origin;
        Vector3 direction;

        Ray() = default;
        Ray(Point3 o, Vector3 d): origin(o), direction(d) {};
    };
}
