#pragma once

#include <cmath>

#include "geometry.hpp"
#include "material.hpp"
#include "ray.hpp"

#define EPS 0.0001
#define INF 1000.0

namespace RayTracer
{
    typedef struct
    {
        bool hit;
        double t;
        Point3 p;
        Object* o;
    } HitRecord;

    class Object
    {
    public:
        TextureMaterial* material;

    public:
        virtual HitRecord hit(Ray ray, double t_min, double t_max) = 0;
        virtual Vector3 normal(Point3 p) = 0;
        virtual MaterialInfo texture(Point3 p) = 0;
    };

    class Sphere : public Object
    {
    public:
        Point3 center;
        double radius;

        Sphere() = default;
        Sphere(Point3 center, double radius, TextureMaterial* mat)
            : center(center), radius(radius)
        {
            material = mat;
        }

    public:
        HitRecord hit(Ray ray, double t_min, double t_max) override
        {
            Vector3 oc = center - ray.origin;
            auto a = ray.direction.norm_squared();
            auto h = ray.direction * oc;
            auto c = oc.norm_squared() - radius * radius;
            auto discriminant = h * h - a * c;

            if (discriminant < 0)
                return {false, 0.0, Point3(), nullptr};

            auto sd = std::sqrt(discriminant);

            auto root = (h - sd) / a;
            if (root <= t_min || root >= t_max)
            {
                root = (h + sd) / a;
                if (root <= t_min || root >= t_max)
                {
                    return {false, 0.0, Point3(), nullptr};
                }
            }

            return {true, root, ray.origin + ray.direction * root, this};
        }

        Vector3 normal(Point3 p) override
        {
            return (p - center).normalize();
        }

        MaterialInfo texture(Point3 p) override
        {
            return material->get_info(
                    p,
                    [this](Point3 p)
                    {
                        return this->mapper(p);
                    }
            );
        }

    private:
        PixelLoc mapper(Point3 p)
        {
            Vector3 n = normal(p);

            double a = M_PI / 2.0;
            Vector3 dir = n;
            dir.x = n.x;
            dir.y = cos(a)*n.y - sin(a)*n.z;
            dir.z = sin(a)*n.y + cos(a)*n.z;

            double longitude = 0.5 + atan2(dir.z, dir.x) / (2.0 * M_PI);
            double latitude = 0.5 + asin(dir.y) / (M_PI);

            longitude += 0.25;
            if (longitude > 1.0) longitude -= 1.0;

            unsigned x = static_cast<unsigned>(
                    longitude * (dynamic_cast<ImageTexture*>(material))->texture.width);
            unsigned y = static_cast<unsigned>(
                    latitude * (dynamic_cast<ImageTexture*>(material))->texture.height);


            return PixelLoc{x, y};
        }
    };
}

