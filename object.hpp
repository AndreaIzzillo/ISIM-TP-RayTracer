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
        const Object* o;
    } HitRecord;

    class Object
    {
    public:
        std::shared_ptr<TextureMaterial> material;
        virtual ~Object() = default;

    public:
        virtual HitRecord hit(const Ray& ray, double t_min, double t_max) const = 0;
    };

    class Sphere : public Object
    {
    public:
        Point3 center;
        double radius;
        Vector3 rotation;

        Sphere() = default;
        Sphere(const Point3& center, double radius, std::shared_ptr<TextureMaterial> mat)
            : center(center), radius(radius)
        {
            material = mat;
        };
        Sphere(const Point3& center, double radius, const Vector3& rotation, std::shared_ptr<TextureMaterial> mat)
            : center(center), radius(radius), rotation(rotation)
        {
            material = mat;
        };

    public:
        HitRecord hit(const Ray& ray, double t_min, double t_max) const override
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
        void rotate(const Vector3& r)
        {
            double pi2 = 2.0 * M_PI;
            rotation.x = std::fmod(rotation.x + r.x, pi2);
            rotation.y = std::fmod(rotation.y + r.y, pi2);
            rotation.z = std::fmod(rotation.z + r.z, pi2);
        }

    private:
        Pixel mapper(const Point3& p) const
        {
            Vector3 dir = (p - center).normalize();

            auto rx = RotXMatrix(rotation.x);
            auto ry = RotYMatrix(rotation.y);
            auto rz = RotZMatrix(rotation.z);

            dir = rx * dir;
            dir = ry * dir;
            dir = rz * dir;

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
        Triangle(const Point3& a, const Point3& b, const Point3& c,
                 std::shared_ptr<TextureMaterial> mat)
            : a(a), b(b), c(c)
        {
            material = mat;
        };

    public:
        HitRecord hit(const Ray& ray, double t_min, double t_max) const override
        {
            Vector3 oa = a - ray.origin;
            Vector3 ob = b - ray.origin;
            Vector3 oc = c - ray.origin;

            Matrix33 m = Matrix33(oa, ob, oc);

            double det = m.determinant();
            double invdet = 1 / det;

            Matrix33 minv = m.inverse();

            Vector3 param = Vector3(
                minv(0, 0) * ray.direction.x + minv(0, 1) * ray.direction.y + minv(0, 2) * ray.direction.z,
                minv(1, 0) * ray.direction.x + minv(1, 1) * ray.direction.y + minv(1, 2) * ray.direction.z,
                minv(2, 0) * ray.direction.x + minv(2, 1) * ray.direction.y + minv(2, 2) * ray.direction.z
            );

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
        Pixel mapper(const Point3& p) const
        {
            return Pixel(0, 0);
        }
    };

    class Mesh : public Object
    {
    public:
        std::vector<std::unique_ptr<Triangle>> triangles;

        Point3 position;
        Vector3 rotation;

        Mesh() = default;
        Mesh(const char *obj_file, const Point3& position, const Vector3& rotation, std::shared_ptr<TextureMaterial> mat)
            : position(position), rotation(rotation)
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

                        auto rx = RotXMatrix(rotation.x);
                        auto ry = RotYMatrix(rotation.y);
                        auto rz = RotZMatrix(rotation.z);

                        tr->a = rx * tr->a;
                        tr->b = rx * tr->b;
                        tr->c = rx * tr->c;

                        tr->a = ry * tr->a;
                        tr->b = ry * tr->b;
                        tr->c = ry * tr->c;

                        tr->a = rz * tr->a;
                        tr->b = rz * tr->b;
                        tr->c = rz * tr->c;

                        tr->a = tr->a + position;
                        tr->b = tr->b + position;
                        tr->c = tr->c + position;

                        triangles.push_back(std::move(tr));
                    }
                }
            }
        };

    public:
        HitRecord hit(const Ray& ray, double t_min, double t_max) const override
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
        void rotate(const Vector3& r)
        {
            double pi2 = 2.0 * M_PI;
            rotation.x = std::fmod(rotation.x + r.x, pi2);
            rotation.y = std::fmod(rotation.y + r.y, pi2);
            rotation.z = std::fmod(rotation.z + r.z, pi2);
        }
        
    private:
        Pixel mapper(const Point3& p) const
        {
            return Pixel(0, 0);
        }
    };
}

