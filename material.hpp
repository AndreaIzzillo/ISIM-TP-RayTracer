#pragma once

#include "geometry.hpp"

namespace RayTracer
{
    typedef struct
    {
        /* Color */
        Color color;

        /* Diffuse */
        double kd;

        /* Specular */
        double ks;
        int ns;
    } MaterialInfo;

    class TextureMaterial
    {
    public:
        virtual MaterialInfo get_info(Point3& p) = 0;
    };

    class UniformTexture : public TextureMaterial
    {
    public:
        MaterialInfo info;

        UniformTexture() = default;
        UniformTexture(Color color, double kd, double ks, int ns)
        {
            info = MaterialInfo{color, kd, ks, ns};
        }

    public:
        MaterialInfo get_info(Point3& p) override
        {
            return info;
        }
    };
}
