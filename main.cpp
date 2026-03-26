#include <cmath>
#include <iostream>
#include <memory>

#include "geometry.hpp"
#include "color.hpp"
#include "image.hpp"
#include "scene.hpp"
#include "material.hpp"
#include "object.hpp"
#include "light.hpp"
#include "camera.hpp"

#define _FD true
#define _DEBUG true

using namespace RayTracer;

Color cast_ray(Ray ray, double t_min, double t_max, Scene& scene,
               std::shared_ptr<Image> background = nullptr, int it = 5)
{
    HitRecord rec = {false, 0.0, Point3(), nullptr};
    Object* hit_object = nullptr;
    double closest = t_max;

    for (auto& object : scene.objects)
    {
        const auto& temp_rec = object->hit(ray, t_min, closest);

        if (temp_rec.hit)
        {
            hit_object = object.get();
            rec = temp_rec;
            closest = temp_rec.t - EPS;
        }
    }

    if (rec.hit)
    {
        const auto& mat = hit_object->texture(rec.p);

        auto result = Color(0.0, 0.0, 0.0);

        auto Id = Color(0.0, 0.0, 0.0); // Diffusion
        auto Is = Color(0.0, 0.0, 0.0); // Specular
        auto Ir = Color(0.0, 0.0, 0.0); // Reflexion

        for (auto& l : scene.lights)
        {
            auto O = ray.direction;
            auto N = hit_object->normal(rec.p);
            auto L = (l->position - rec.p).normalize();
            auto S = (O - N * 2 * (O * N)).normalize();

            bool shadow = false;
            for (auto& object : scene.objects)
            {
                auto rec_s = object->hit({rec.p + N * EPS, L}, 0.0, INF);
                if (rec_s.hit &&
                    rec.o != object.get() &&
                    (rec.p - rec_s.p).norm() < (l->position - rec.p).norm())
                {
                    shadow = true;
                    break;
                }
            }

            if (shadow)
                continue;

            double fd = _FD ? 1.0 / sqrt((l->position - rec.p).norm()) : 1.0;

            if (mat.kd > 0.0)
            {
                Id = Id + mat.color * mat.kd * std::max((N * L), 0.125) * l->color * l->intensity * fd;
            }

            if (mat.ks > 0.0)
            {
                Is = Is + l->color * mat.ks * pow(std::max((S * L), 0.0), mat.ns) * l->intensity * fd;

                if (it > 0)
                {
                    Ir = cast_ray({rec.p + N * EPS, S}, 0.0, INF, scene, background, it - 1) * mat.ks;
                }
            }

            result = result + Id + Is + Ir;
        }

        return result;
    }
    else
    {
        if (background)
        {
            Vector3 n = ray.direction;

            double a = M_PI / 2.0;
            Vector3 dir = n;
            dir.x = n.x;
            dir.y = cos(a)*n.y - sin(a)*n.z;
            dir.z = sin(a)*n.y + cos(a)*n.z;

            dir = -dir;

            double longitude = 0.5 + atan2(dir.z, dir.x) / (2.0 * M_PI);
            double latitude = 0.5 + asin(dir.y) / (M_PI);

            longitude += 0.25;
            if (longitude > 1.0) longitude -= 1.0;

            unsigned x = static_cast<unsigned>(longitude * background->width);
            unsigned y = static_cast<unsigned>(latitude * background->height);

            return background->get_pixel(x, y);
        }
        else
        {
            return CYAN * 0.5;
        }
    }
}

Image generate_image(Camera& cam, Scene& scene, std::shared_ptr<Image> background = nullptr)
{
    Image img = Image(cam.w_res, cam.h_res);

    for (unsigned y = 0; y < cam.h_res; y++)
    {
        for (unsigned x = 0; x < cam.w_res; x++)
        {
            Ray ray = cam.get_ray(x, y);
            Color color = cast_ray(ray, 0.0, INF, scene, background);
            img.set_pixel(x, y, color);
        }

        if (_DEBUG)
            std::cout << "Line " << y << " done." << "    " << "\r";
    }

    if (_DEBUG)
        std::cout << std::endl;

    return img;
}

int main(int argc, char **argv)
{
    auto scene = Scene();

    auto camera = Camera(
        Point3(0.0, -3.0, 1.0), Point3(0.0, 0.0, 0.5),
        M_PI / 3.0, M_PI / 4.0, 1.0, 500
    );

    auto tex = std::make_shared<Image>("texture/jupiter.ppm");

    auto cyan = std::make_shared<UniformTexture>(BLUE, 0.0, 1.0, 10.0);
    auto magenta = std::make_shared<UniformTexture>(MAGENTA, 0.0, 1.0, 10.0);
    auto boule = std::make_shared<UniformTexture>(RED, 1.0, 0.0, 10.0);
    auto white = std::make_shared<UniformTexture>(WHITE, 0.9, 0.1, 10.0);
    auto chiant = std::make_shared<ImageTexture>(tex, 1.0, 0.0, 10.0);

    auto tr1 = std::make_unique<Triangle>(
            Point3(-1.5, 1.0, 0.0),
            Point3(-0.5, 2.0, 0.0),
            Point3(-1.0, 1.5, 1.0),
            cyan
    );
    auto tr2 = std::make_unique<Triangle>(
            Point3(0.5, 2.0, 0.0),
            Point3(1.5, 1.0, 0.0),
            Point3(1.0, 1.5, 1.0),
            magenta
    );
    auto cool = std::make_unique<Sphere>(
            Point3(0.0, 1.0, 0.3), 0.3, chiant
    );
    auto ground = std::make_unique<Sphere>(
            Point3(0.0, 0.0, -10000.0), 10000.00, white
    );

    scene.add_object(std::move(tr1));
    scene.add_object(std::move(tr2));
    scene.add_object(std::move(cool));
    scene.add_object(std::move(ground));
    scene.add_light(std::make_unique<PointLight>(Point3(0.0, 0.0, 1.0), WHITE, 1.0));

    generate_image(camera, scene).to_ppm("result.ppm");

    return 0;
}
