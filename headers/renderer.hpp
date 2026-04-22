#pragma once

#include <memory>

#include "camera.hpp"
#include "color.hpp"
#include "geometry.hpp"
#include "image.hpp"
#include "light.hpp"
#include "material.hpp"
#include "object.hpp"
#include "scene.hpp"

#define LIGHT_DISTANCE true
#define REFLEXION_IT 5

namespace RayTracer {
class Renderer {
public:
    std::unique_ptr<Scene> scene;
    std::unique_ptr<Camera> cam;

    Renderer() = delete;
    Renderer(std::unique_ptr<Camera> camera, std::unique_ptr<Scene> scene)
        : cam(std::move(camera))
        , scene(std::move(scene)) { };

public:
    Image generate_image(std::shared_ptr<Image> background = nullptr)
    {
        Image img = Image(cam->w_res, cam->h_res);

        std::cout << "Rendering..." << std::endl;

#pragma omp parallel for collapse(2) schedule(dynamic, 1)
        for (unsigned y = 0; y < cam->h_res; y++) {
            for (unsigned x = 0; x < cam->w_res; x++) {
                Ray ray = cam->get_ray(x, y);
                Color color = cast_ray(ray, 0.0, INF, *scene, background);
                img.set_pixel(x, y, color);
            }
        }

        std::cout << "Done." << std::endl;

        return img;
    }

private:
    Color cast_ray(const Ray& ray, double t_min, double t_max, const Scene& scene,
        std::shared_ptr<Image> background = nullptr, int it = REFLEXION_IT)
    {
        HitRecord rec = { false };
        Object* hit_object = nullptr;
        double closest = t_max;

        for (auto& object : scene.objects) {
            const auto& temp_rec = object->hit(ray, t_min, closest);

            if (temp_rec.hit) {
                hit_object = object.get();
                rec = temp_rec;
                closest = temp_rec.t - EPS;
            }
        }

        if (rec.hit) {
            const auto& mat = rec.mat;

            auto result = Color(0.0, 0.0, 0.0);

            auto Id = Color(0.0, 0.0, 0.0); // Diffusion
            auto Is = Color(0.0, 0.0, 0.0); // Specular
            auto Ir = Color(0.0, 0.0, 0.0); // Reflexion

            auto O = ray.direction;
            auto N = rec.n;
            auto S = (O - N * 2 * (O * N)).normalize();

            for (auto& l : scene.lights) {
                auto L = (l->position - rec.p).normalize();

                double fd = LIGHT_DISTANCE ? 1.0 / (l->position - rec.p).norm() : 1.0;

                bool is_shadow = false;
                for (auto& object : scene.objects) {
                    auto rec_s = object->hit({ rec.p + N * EPS, L }, 0.0, INF);
                    if (rec_s.hit && rec.o != object.get() && (rec.p - rec_s.p).norm() < (l->position - rec.p).norm()) {
                        is_shadow = true;
                        break;
                    }
                }

                if (is_shadow)
                    continue;

                if (mat.kd > 0.0) {
                    Id = Id + mat.color * mat.kd * std::max((N * L), 0.125) * l->color * l->intensity * fd;
                }

                if (mat.ks > 0.0) {
                    Is = Is + l->color * mat.ks * pow(std::max((S * L), 0.0), mat.ns) * l->intensity * fd;
                }
            }

            if (mat.ks > 0.0) {
                if (it > 0) {
                    Ir = Ir + cast_ray({ rec.p + N * EPS, S }, 0.0, INF, scene, background, it - 1) * mat.ks;
                }
            }

            result = result + Id + Is + Ir;

            return result;
        } else {
            if (background) {
                Vector3 n = ray.direction;

                double a = M_PI / 2.0;
                Vector3 dir = n;
                dir.x = n.x;
                dir.y = cos(a) * n.y - sin(a) * n.z;
                dir.z = sin(a) * n.y + cos(a) * n.z;

                dir = -dir;

                double longitude = 0.5 + atan2(dir.z, dir.x) / (2.0 * M_PI);
                double latitude = 0.5 + asin(dir.y) / (M_PI);

                longitude += 0.25;
                if (longitude > 1.0)
                    longitude -= 1.0;

                unsigned x = static_cast<unsigned>(longitude * background->width);
                unsigned y = static_cast<unsigned>(latitude * background->height);

                return background->get_pixel(x, y);
            } else {
                return Color(135, 206, 235);
            }
        }
    }
};
}