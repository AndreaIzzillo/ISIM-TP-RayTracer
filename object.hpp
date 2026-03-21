#pragma once

#include <cmath>

#include "geometry.hpp"
#include "material.hpp"

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
        std::shared_ptr<TextureMaterial> material;
        virtual ~Object() = default;

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

        double x_rot = 0.0;
        double y_rot = 0.0;
        double z_rot = 0.0;

        Sphere() = default;
        Sphere(Point3 center, double radius, std::shared_ptr<TextureMaterial> mat)
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
    
    public:
        void rotate(double x, double y, double z)
        {
            double pi2 = 2.0 * M_PI;
            x_rot = std::fmod(x_rot + x, pi2);
            y_rot = std::fmod(y_rot + y, pi2);
            z_rot = std::fmod(z_rot + z, pi2);
        }

    private:
        Pixel mapper(Point3 p)
        {
            Vector3 dir = normal(p);

            double cx = std::cos(x_rot);
            double sx = std::sin(x_rot);
            double cy = std::cos(y_rot);
            double sy = std::sin(y_rot);
            double cz = std::cos(z_rot);
            double sz = std::sin(z_rot);
            double x0, y0, z0;

            // X rotation
            y0 = dir.y;
            z0 = dir.z;
            dir.y = cx * y0 - sx * z0;
            dir.z = sx * y0 + cx * z0;
            // Y rotation
            x0 = dir.x;
            z0 = dir.z;
            dir.x = cy * x0 + sy * z0;
            dir.z = -sy * x0 + cy * z0;
            // Z rotation
            x0 = dir.x;
            y0 = dir.y;
            dir.x = cz * x0 - sz * y0;
            dir.y = sz * x0 + cz * y0;

            double longitude = 0.5 - atan2(dir.y, dir.x) / (2.0 * M_PI);
            double latitude = 0.5 + asin(dir.z) / (M_PI);

            longitude += 0.25;
            if (longitude > 1.0) longitude -= 1.0;

            unsigned x = static_cast<unsigned>(
                    longitude * (dynamic_cast<ImageTexture*>(material.get()))->texture->width);
            unsigned y = static_cast<unsigned>(
                    latitude * (dynamic_cast<ImageTexture*>(material.get()))->texture->height);


            return Pixel{x, y};
        }
    };
}

