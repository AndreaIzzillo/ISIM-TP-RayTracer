#pragma once

#include <vector>

namespace RayTracer
{
    class Object;
    class Light;

    class Scene
    {
    public:
        std::vector<std::unique_ptr<Object>> objects;
        std::vector<std::unique_ptr<Light>> lights;

        Scene() = default;
        ~Scene() = default;

    public:
        void add_object(std::unique_ptr<Object> obj)
        {
            objects.push_back(std::move(obj));
        }

        void add_light(std::unique_ptr<Light> light)
        {
            lights.push_back(std::move(light));
        }
    };
}
