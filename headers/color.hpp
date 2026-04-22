#pragma once

#include <ostream>

#define WHITE Color(1.0, 1.0, 1.0)
#define BLACK Color(0.0, 0.0, 0.0)
#define RED Color(1.0, 0.0, 0.0)
#define GREEN Color(0.0, 1.0, 0.0)
#define BLUE Color(0.0, 0.0, 1.0)
#define CYAN Color(0.0, 1.0, 1.0)
#define MAGENTA Color(1.0, 0.0, 1.0)
#define YELLOW Color(1.0, 1.0, 0.0)

namespace RayTracer {
class Color {
public:
    double r, g, b;

    Color()
        : r(0.0)
        , g(0.0)
        , b(0.0) { };
    Color(double r, double g, double b)
        : r(r)
        , g(g)
        , b(b) { };
    Color(int r, int g, int b)
        : r(static_cast<double>(r) / 255.0)
        , g(static_cast<double>(g) / 255.0)
        , b(static_cast<double>(b) / 255.0) { };

public:
    Color operator*(const double& l) const
    {
        return Color(r * l, g * l, b * l);
    }

    Color operator+(const Color& c) const
    {
        return Color(r + c.r, g + c.g, b + c.b);
    }

    Color operator-(const Color& c) const
    {
        return Color(r - c.r, g - c.g, b - c.b);
    }

    Color operator*(const Color& c) const
    {
        return Color(r * c.r, g * c.g, b * c.b);
    }

    friend std::ostream& operator<<(std::ostream& out, Color& c)
    {
        return out << "C(" << c.r << "," << c.g << "," << c.b << ")";
    }

public:
    Color clamp() const
    {
        return Color(
            r < 0.0 ? 0.0 : (r > 1.0 ? 1.0 : r),
            g < 0.0 ? 0.0 : (g > 1.0 ? 1.0 : g),
            b < 0.0 ? 0.0 : (b > 1.0 ? 1.0 : b));
    }
};
}
