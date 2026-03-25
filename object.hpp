#pragma once

#include <cmath>
#include <memory>

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
        };

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


            return Pixel(x, y);
        }
    };

    class Triangle : public Object
    {
    public:
        Point3 a;
        Point3 b;
        Point3 c;

        Triangle() = default;
        Triangle(Point3 a, Point3 b, Point3 c,
                 std::shared_ptr<TextureMaterial> mat)
            : a(a), b(b), c(c)
        {
            material = mat;
        };

    public:
        HitRecord hit(Ray ray, double t_min, double t_max) override
        {
            Vector3 oa = a - ray.origin;
            Vector3 ob = b - ray.origin;
            Vector3 oc = c - ray.origin;

            double det = oa.x * (ob.y * oc.z - oc.y * ob.z) -
                         oa.y * (ob.x * oc.z - ob.z * oc.x) +
                         oa.z * (ob.x * oc.y - ob.y * oc.x);

            double invdet = 1 / det;

            Vector3 m0;
            Vector3 m1;
            Vector3 m2;

            m0.x = (ob.y * oc.z - oc.y * ob.z) * invdet;
            m0.y = (oa.z * oc.y - oa.y * oc.z) * invdet;
            m0.z = (oa.y * ob.z - oa.z * ob.y) * invdet;
            m1.x = (ob.z * oc.x - ob.x * oc.z) * invdet;
            m1.y = (oa.x * oc.z - oa.z * oc.x) * invdet;
            m1.z = (ob.x * oa.z - oa.x * ob.z) * invdet;
            m2.x = (ob.x * oc.y - oc.x * ob.y) * invdet;
            m2.y = (oc.x * oa.y - oa.x * oc.y) * invdet;
            m2.z = (oa.x * ob.y - ob.x * oa.y) * invdet;

            Vector3 param;
            param.x = m0.x * ray.direction.x + m1.x * ray.direction.y + m2.x * ray.direction.z;
            param.y = m0.y * ray.direction.x + m1.y * ray.direction.y + m2.y * ray.direction.z;
            param.z = m0.z * ray.direction.x + m1.z * ray.direction.y + m2.z * ray.direction.z;

            if (param.x < 0.0 || param.y < 0.0 || param.z < 0.0)
                return {false, 0.0, Point3(), nullptr};

            Vector3 og = (oa * param.x + ob * param.y + oc * param.z);
            og = og / (param.x + param.y + param.z);

            if (og.norm() < t_min || og.norm() > t_max)
                return {false, 0.0, Point3(), nullptr};

            return {true, og.norm(), ray.origin + og, this};
        }

        Vector3 normal(Point3 p) override
        {
            return (a-b)^(a-c);
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
        Pixel mapper(Point3 p)
        {
            return Pixel(0, 0);
        }
    };
}

