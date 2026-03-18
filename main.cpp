#include <cmath>
#include <iostream>

#include "geometry.hpp"
#include "color.hpp"
#include "image.hpp"
#include "scene.hpp"
#include "material.hpp"
#include "ray.hpp"
#include "object.hpp"
#include "light.hpp"
#include "camera.hpp"

#define EPS 0.0001
#define INF 1000.0

#define WHITE Color(1.0, 1.0, 1.0)
#define RED Color(1.0, 0.0, 0.0)
#define GREEN Color(0.0, 1.0, 0.0)
#define BLUE Color(0.0, 0.0, 1.0)
#define CYAN Color(0.0, 1.0, 1.0)
#define MAGENTA Color(1.0, 0.0, 1.0)
#define YELLOW Color(1.0, 1.0, 0.0)

using namespace RayTracer;

Color cast_ray(Ray ray, double t_min, double t_max, Scene scene, int it = 5)
{
    HitRecord rec = {false, 0.0, Point3(), nullptr};
    Object* hit_object = nullptr;
    double closest = t_max;

    for (auto object : scene.objects)
    {
        auto temp_rec = object->hit(ray, t_min, closest);

        if (temp_rec.hit)
        {
            hit_object = object;
            rec = temp_rec;
            closest = temp_rec.t - EPS;
        }
    }

    if (rec.hit)
    {
        auto mat = hit_object->texture(rec.p);

        auto result = Color(0.0, 0.0, 0.0);

        auto Id = Color(0.0, 0.0, 0.0); // Diffusion
        auto Is = Color(0.0, 0.0, 0.0); // Specular
        auto Ir = Color(0.0, 0.0, 0.0); // Reflexion

        for (auto l : scene.lights)
        {
            auto O = ray.direction;
            auto N = hit_object->normal(rec.p);
            auto L = (l->position - rec.p).normalize();
            auto S = (O - N * 2 * (O * N)).normalize();

            bool shadow = false;
            for (auto object : scene.objects)
            {
                auto rec_s = object->hit({rec.p + N * EPS, L}, 0.0, INF);
                if (rec_s.hit &&
                    rec.o != object &&
                    (rec.p - rec_s.p).norm() < (l->position - rec.p).norm())
                {
                    shadow = true;
                    break;
                }
            }

            if (shadow)
                continue;

            double fd = 1 / (l->position - rec.p).norm();

            if (mat.kd > 0.0)
            {
                Id = Id + mat.color * mat.kd * std::max((N * L), 0.0) * l->color * l->intensity * fd;
            }

            if (mat.ks > 0.0)
            {
                Is = Is + l->color * mat.ks * pow(std::max((S * L), 0.0), mat.ns) * l->intensity * fd;

                if (it > 0)
                {
                    Ir = cast_ray({rec.p + N * EPS, S}, 0.0, INF, scene, it - 1) * mat.ks;
                }
            }

            result = result + Id + Is + Ir;
        }

        return result;
    }
    else
    {
        return Color(0.0, 0.4, 0.6);
    }
}

Image generate_image(Camera cam, Scene scene)
{
    Image img = Image(cam.w_res, cam.h_res);

    for (unsigned y = 0; y < cam.h_res; y++)
    {
        for (unsigned x = 0; x < cam.w_res; x++)
        {
            Ray ray = cam.get_ray(x, y);
            Color color = cast_ray(ray, 0.0, INF, scene);
            img.set_pixel(x, y, color);
        }
    }

    return img;
}

int main(int argc, char **argv)
{
    auto scene = Scene();

    auto camera = Camera(
        Point3(0.0, -4.0, 2.0), Point3(0.0, 1.0, 0.0),
        M_PI / 3.0, M_PI / 4.0, 2.0, 800
    );

    auto earth_tex = Image("texture/earth.ppm");

    auto blue_mat = UniformTexture(Color(0.0, 1.0, 1.0), 0.5, 0.5, 10.0);
    auto red_mat = UniformTexture(Color(1.0, 0.2, 0.2), 1.0, 0.0, 10.0);
    auto green_mat = UniformTexture(Color(0.3, 1.0, 0.1), 0.7, 0.3, 10.0);
    auto metallic_mat = UniformTexture(WHITE, 0.1, 0.9, 10.0);
    auto gray_mat = UniformTexture(Color(0.3, 0.3, 0.3), 0.9, 0.1, 10.0);
    auto earth_mat = ImageTexture(earth_tex, 1.0, 0.0, 10.0);

    Sphere o1 = Sphere(Point3(1.5, 0.0, 0.5), 0.5, &blue_mat);
    Sphere o2 = Sphere(Point3(-1.5, 0.0, 0.5), 0.5, &red_mat);
    Sphere o3 = Sphere(Point3(0.0, -1.5, 0.5), 0.5, &metallic_mat);
    Sphere o4 = Sphere(Point3(0.0, 0.0, 0.8), 0.8, &earth_mat);

    Sphere gr = Sphere(Point3(0.0, 0.0, -5000.0), 5000.0, &gray_mat);

    scene.add_object(&o1);
    scene.add_object(&o2);
    scene.add_object(&o3);
    scene.add_object(&o4);

    scene.add_object(&gr);

    //PointLight l1 = PointLight(Point3(1.0, -1.5, 2.0), Color(1.0, 0.8, 0.5), 0.6);
    //PointLight l2 = PointLight(Point3(-1.0, -1.5, 2.0), Color(1.0, 0.8, 0.5), 0.6);
    PointLight l1 = PointLight(Point3(1.0, -1.5, 3.0), WHITE, 1.0);
    PointLight l2 = PointLight(Point3(-1.0, -1.5, 3.0), WHITE, 1.0);

    scene.add_light(&l1);
    scene.add_light(&l2);

    Image img = generate_image(camera, scene);

    img.to_ppm("result.ppm");

    return 0;
}
