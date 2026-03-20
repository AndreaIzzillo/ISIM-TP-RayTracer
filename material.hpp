#pragma once

#include <functional>

#include "geometry.hpp"

namespace RayTracer
{
    class Object;

    typedef struct
    {
        Color color; // Diffuse
        double kd;   // Diffuse
        double ks;   // Specular
        int ns;      // Specular
    } MaterialInfo;

    class TextureMaterial
    {
    public:
        MaterialInfo info;
    
    public:
        virtual MaterialInfo get_info(
                Point3& p,
                std::function<Pixel(Point3 p)> mapper) = 0;
    };

    class UniformTexture : public TextureMaterial
    {
    public:
        UniformTexture() = default;
        UniformTexture(Color color, double kd, double ks, int ns)
        {
            info = MaterialInfo{color, kd, ks, ns};
        }

    public:
        MaterialInfo get_info(
                Point3& p,
                std::function<Pixel(Point3 p)> mapper) override
        {
            return info;
        }
    };

    class ImageTexture : public TextureMaterial
    {
    public:
        std::shared_ptr<Image> texture;

        ImageTexture(std::shared_ptr<Image> img, double kd, double ks, int ns) : texture(img)
        {
            info = MaterialInfo{Color(1.0, 1.0, 1.0), kd, ks, ns};
        }

    public:
        MaterialInfo get_info(
                Point3& p,
                std::function<Pixel(Point3 p)> mapper) override
        {
            auto ret = info;
            auto px = mapper(p);
            ret.color = texture->get_pixel(px.x, px.y);
            return ret;
        }
    };
}
