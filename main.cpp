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
        Point3(0.0, -3.0, 0.5), Point3(0.0, 0.0, 1.0),
        M_PI / 3.0, M_PI / 4.0, 1.0, 1000
    );

    auto white = std::make_shared<UniformTexture>(WHITE * 0.95, 1.0, 0.0, 10.0);
    auto cyan = std::make_shared<UniformTexture>(CYAN, 1.0, 0.0, 10.0);

    auto ground = std::make_unique<Sphere>(
            Point3(0.0, 0.0, -10000.0), 10000.0, white
    );

    // auto suzanne = std::make_unique<Mesh>(
    //     "objects/suzanne.obj",
    //     Point3(2.5, 4.5, 0.0),
    //     Vector3(M_PI_2, 0.0, 0.0),
    //     white
    // );

    auto suzanne = std::make_unique<Mesh>(
        "objects/bus.obj",
        Point3(-1.0, 8.0, -0.0),
        Vector3(0.0, 0.0, M_PI_4),
        white
    );

    scene->add_object(std::move(ground));
    scene->add_object(std::move(suzanne));

    scene->add_light(std::make_unique<PointLight>(Point3(0.0, 5.0, 10.0), WHITE, 1.0));
    scene->add_light(std::make_unique<PointLight>(Point3(-4.0, 5.0, 2.0), Color(75,192,173), 1.2));
    scene->add_light(std::make_unique<PointLight>(Point3(0.0, 2.0, 2.0), BLUE, 1.2));
    scene->add_light(std::make_unique<PointLight>(Point3(4.0, 3.0, 2.0), MAGENTA, 1.2));

    auto renderer = Renderer(std::move(camera), std::move(scene));
    renderer.generate_image().to_ppm("result.ppm");

    return 0;
}
