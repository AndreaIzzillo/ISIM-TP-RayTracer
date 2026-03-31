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
#include "renderer.hpp"

using namespace RayTracer;

int main(int argc, char **argv)
{
    auto scene = std::make_unique<Scene>();
    auto camera = std::make_unique<Camera>(
        Point3(0.0, -3.0, 1.0), Point3(0.0, 0.0, 0.5),
        M_PI / 3.0, M_PI / 4.0, 1.0, 600
    );

    auto white = std::make_shared<UniformTexture>(WHITE * 0.95, 1.0, 0.0, 10.0);

    auto ground = std::make_unique<Sphere>(
            Point3(0.0, 0.0, -1000.0), 1000.0, white
    );

    auto plant = std::make_unique<Mesh>(
        "objects/plant.obj",
        Point3(0.0, 0.0, 0.0),
        Vector3(M_PI_2, 0.0, 0.0),
        0.04,
        white
    );

    scene->add_object(std::move(ground));
    scene->add_object(std::move(plant));

    scene->add_light(std::make_unique<PointLight>(
        Point3(0.0, -1.0, 2.0),
        WHITE,
        1.2
    ));

    auto renderer = Renderer(std::move(camera), std::move(scene));
    renderer.generate_image().to_ppm("result.ppm");

    return 0;
}
