#pragma once

#include "color.hpp"
#include "geometry.hpp"

namespace RayTracer {
class Light {
public:
    Point3 position;
    Color color;
    double intensity;
};

class PointLight : public Light {
public:
    PointLight() = default;
    PointLight(const Point3& p, const Color& c, double i)
    {
        position = p;
        color = c;
        intensity = i;
    }
};
}
