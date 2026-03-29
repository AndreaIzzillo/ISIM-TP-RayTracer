#pragma once

#include <cmath>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "geometry.hpp"
#include "material.hpp"

namespace RayTracer
{
    typedef struct
    {
        bool hit;
        double t;
        Point3 p;
        Vector3 n;
        MaterialInfo mat;
        Object* o;
    } HitRecord;

    class Object
    {
    public:
        std::shared_ptr<TextureMaterial> material;
        virtual ~Object() = default;

    public:
        virtual HitRecord hit(Ray ray, double t_min, double t_max) = 0;
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
                return {false};

            auto sd = std::sqrt(discriminant);

            auto root = (h - sd) / a;
            if (root <= t_min || root >= t_max)
            {
                root = (h + sd) / a;
                if (root <= t_min || root >= t_max)
                {
                    return {false};
                }
            }

            auto p = ray.origin + ray.direction * root;
            auto mat = material->get_info(
                p,
                [this](Point3 p)
                {
                    return this->mapper(p);
                }
            );

            return {
                true, 
                root,
                p,
                (p - center).normalize(),
                mat,
                this
            };
        }

    public:
        void rotate(double rx, double ry, double rz)
        {
            double pi2 = 2.0 * M_PI;
            x_rot = std::fmod(x_rot + rx, pi2);
            y_rot = std::fmod(y_rot + ry, pi2);
            z_rot = std::fmod(z_rot + rz, pi2);
        }

    private:
        Pixel mapper(Point3 p)
        {
            Vector3 dir = (p - center).normalize();

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
                    longitude * (dynamic_cast<ImageTexture*>(material.get()))->texture->width
            );
            unsigned y = static_cast<unsigned>(
                    latitude * (dynamic_cast<ImageTexture*>(material.get()))->texture->height
            );

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
                return {false};

            Vector3 og = (oa * param.x + ob * param.y + oc * param.z);
            og = og / (param.x + param.y + param.z);

            double t = og * ray.direction;

            if (t < t_min || t > t_max)
                return {false};

            auto p = ray.origin + ray.direction * t;
            auto mat = material->get_info(
                    p,
                    [this](Point3 p)
                    {
                        return this->mapper(p);
                    }
            );

            return {
                true,
                t,
                p,
                ((b-a)^(c-a)).normalize(),
                mat,
                this
            };
        }

    private:
        Pixel mapper(Point3 p)
        {
            return Pixel(0, 0);
        }
    };

    class Mesh : public Object
    {
    public:
        Point3 position;
        std::vector<std::unique_ptr<Triangle>> triangles;

        double x_rot = 0.0;
        double y_rot = 0.0;
        double z_rot = 0.0;

        Mesh() = default;
        Mesh(const char *obj_file, Point3 pos, Vector3 rot, std::shared_ptr<TextureMaterial> mat)
            : position(pos), x_rot(rot.x), y_rot(rot.y), z_rot(rot.z)
        {
            material = mat;

            auto file = std::ifstream(obj_file);

            std::vector<Point3> v;
            std::vector<Vector3> vn;

            std::string line;
            while (std::getline(file, line))
            {
                auto ss = std::stringstream(line);

                std::string type;
                ss >> type;

                /* Vertex */
                if (type == "v")
                {
                    std::vector<std::string> coords;
                    std::string c;

                    while (ss >> c)
                        coords.push_back(c);
                    
                    if (coords.size() != 3)
                        throw std::runtime_error("Non-triangle mesh.");

                    v.push_back(Point3(
                        atof(coords[0].c_str()),
                        atof(coords[1].c_str()),
                        atof(coords[2].c_str())
                    ));
                }

                /* Normal */
                if (type == "vn")
                {
                    std::vector<std::string> coords;
                    std::string c;

                    while (ss >> c)
                        coords.push_back(c);

                    if (coords.size() != 3)
                        throw std::runtime_error("Non-3D normal vector.");

                    vn.push_back(Vector3(
                        atof(coords[0].c_str()),
                        atof(coords[1].c_str()),
                        atof(coords[2].c_str())
                    ));
                }

                /* Texture */
                if (type == "vt")
                {
                    // Not implemented
                }

                /* Triangle */
                if (type == "f")
                {
                    std::vector<std::string> values;
                    std::string val;

                    while (ss >> val)
                        values.push_back(val);

                    if (values.size() < 3)
                        throw std::runtime_error("Non-triangle mesh.");
                    
                    struct Indices
                    {
                        int v  = -1;
                        int vt = -1;
                        int vn = -1;
                    };

                    std::vector<Indices> indices;
                    for (auto& val : values)
                    {
                        Indices idx;

                        auto val_ss = std::stringstream(val);
                        std::string item;

                        if (std::getline(val_ss, item, '/'))
                            if (!item.empty())
                                idx.v = std::stoi(item.c_str());

                        if (std::getline(val_ss, item, '/'))
                            if (!item.empty())
                                idx.vt = std::stoi(item.c_str());

                        if (std::getline(val_ss, item, '/'))
                            if (!item.empty())
                                idx.vn = std::stoi(item.c_str());
                        
                        indices.push_back(idx);
                    }

                    for (int i = 1; i < indices.size() - 1; i++)
                    {
                        auto tr = std::make_unique<Triangle>(Triangle(
                            v[indices[0].v - 1],
                            v[indices[i].v - 1],
                            v[indices[i + 1].v - 1],
                            material
                        ));

                        double cx = std::cos(x_rot);
                        double sx = std::sin(x_rot);
                        double cy = std::cos(y_rot);
                        double sy = std::sin(y_rot);
                        double cz = std::cos(z_rot);
                        double sz = std::sin(z_rot);

                        double x0, y0, z0;

                        // X rotation
                        y0 = tr->a.y;
                        z0 = tr->a.z;
                        tr->a.y = cx * y0 - sx * z0;
                        tr->a.z = sx * y0 + cx * z0;

                        // Y rotation
                        x0 = tr->a.x;
                        z0 = tr->a.z;
                        tr->a.x = cy * x0 + sy * z0;
                        tr->a.z = -sy * x0 + cy * z0;

                        // Z rotation
                        x0 = tr->a.x;
                        y0 = tr->a.y;
                        tr->a.x = cz * x0 - sz * y0;
                        tr->a.y = sz * x0 + cz * y0;

                        // X rotation
                        y0 = tr->b.y;
                        z0 = tr->b.z;
                        tr->b.y = cx * y0 - sx * z0;
                        tr->b.z = sx * y0 + cx * z0;

                        // Y rotation
                        x0 = tr->b.x;
                        z0 = tr->b.z;
                        tr->b.x = cy * x0 + sy * z0;
                        tr->b.z = -sy * x0 + cy * z0;

                        // Z rotation
                        x0 = tr->b.x;
                        y0 = tr->b.y;
                        tr->b.x = cz * x0 - sz * y0;
                        tr->b.y = sz * x0 + cz * y0;

                        // X rotation
                        y0 = tr->c.y;
                        z0 = tr->c.z;
                        tr->c.y = cx * y0 - sx * z0;
                        tr->c.z = sx * y0 + cx * z0;

                        // Y rotation
                        x0 = tr->c.x;
                        z0 = tr->c.z;
                        tr->c.x = cy * x0 + sy * z0;
                        tr->c.z = -sy * x0 + cy * z0;

                        // Z rotation
                        x0 = tr->c.x;
                        y0 = tr->c.y;
                        tr->c.x = cz * x0 - sz * y0;
                        tr->c.y = sz * x0 + cz * y0;

                        tr->a = tr->a + position;
                        tr->b = tr->b + position;
                        tr->c = tr->c + position;

                        triangles.push_back(std::move(tr));
                    }
                }
            }
        };

    public:
        HitRecord hit(Ray ray, double t_min, double t_max) override
        {
            HitRecord rec = {false};

            double closest = t_max;
            for (int i = 0; i < triangles.size(); i++)
            {
                Triangle* current = triangles[i].get();
                auto temp_rec = current->hit(ray, t_min, closest);
                if (!temp_rec.hit)
                    continue;
                
                closest = temp_rec.t;
                rec = temp_rec;
            }

            return rec;
        }
    
    public:
        void rotate(double rx, double ry, double rz)
        {
            double pi2 = 2.0 * M_PI;
            x_rot = std::fmod(x_rot + rx, pi2);
            y_rot = std::fmod(y_rot + ry, pi2);
            z_rot = std::fmod(z_rot + rz, pi2);
        }
        
    private:
        Pixel mapper(Point3 p)
        {
            return Pixel(0, 0);
        }
    };
}

