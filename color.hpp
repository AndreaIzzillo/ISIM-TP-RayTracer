#pragma once

#include <ostream>

namespace RayTracer
{
    class Color
    {
    public:
        double r, g, b;

        Color(): r(0.0), g(0.0), b(0.0) {};
        Color(double r, double g, double b): r(r), g(g), b(b)
        {
            span();
        };

    public:
        Color operator*(const double& l) const
        {
            auto res = Color(r * l, g * l, b * l);
            res.span();
            return res;
        }

        Color operator+(const Color& c) const
        {
            auto res = Color(r + c.r, g + c.g, b + c.b);
            res.span();
            return res;
        }

        Color operator-(const Color& c) const
        {
            auto res = Color(r - c.r, g - c.g, b - c.b);
            res.span();
            return res;
        }

        Color operator*(const Color& c) const
        {
            auto res = Color(r * c.r, g * c.g, b * c.b);
            res.span();
            return res;
        }

        friend std::ostream& operator<<(std::ostream& out, Color& c)
        {
            return out << "C(" << c.r << "," << c.g << "," << c.b << ")";
        }

    private:
        void span()
        {
            if (r < 0.0) r = 0.0;
            if (r > 1.0) r = 1.0;
            if (g < 0.0) g = 0.0;
            if (g > 1.0) g = 1.0;
            if (b < 0.0) b = 0.0;
            if (b > 1.0) b = 1.0;
        }
    };
}
