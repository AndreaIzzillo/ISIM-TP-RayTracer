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
            return material->get_info(p);
        }
    };
}

