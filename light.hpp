#pragma once

#include "geometry.hpp"
#include "color.hpp"

namespace RayTracer
{
    class Light
    {
    public:
        Point3 position;
        Color color;
        double intensity;
    };

    class PointLight : public Light
    {
    public:
        PointLight() = default;
        PointLight(Point3 p, Color c, double i)
        {
            position = p;
            color = c;
            intensity = i;
        }
    };
}
