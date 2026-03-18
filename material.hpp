#pragma once

#include <functional>

#include "geometry.hpp"

namespace RayTracer
{
    class Object;

    typedef struct
    {
        unsigned x, y;
    } PixelLoc;

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
        virtual MaterialInfo get_info(
                Point3& p,
                std::function<PixelLoc(Point3 p)> mapper) = 0;
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
        MaterialInfo get_info(
                Point3& p,
                std::function<PixelLoc(Point3 p)> mapper) override
        {
            return info;
        }
    };

    class ImageTexture : public TextureMaterial
    {
    public:
        MaterialInfo info;
        Image& texture;

        ImageTexture(Image& img, double kd, double ks, int ns) : texture(img)
        {
            info = MaterialInfo{Color(1.0, 1.0, 1.0), kd, ks, ns};
        }

    public:
        MaterialInfo get_info(
                Point3& p,
                std::function<PixelLoc(Point3 p)> mapper) override
        {
            auto ret = info;
            auto px = mapper(p);
            ret.color = texture.get_pixel(px.x, px.y);
            return ret;
        }
    };
}
