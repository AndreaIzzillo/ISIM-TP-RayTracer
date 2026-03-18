#pragma once

#include <vector>

namespace RayTracer
{
    class Object;
    class Light;

    class Scene
    {
    public:
        std::vector<Object*> objects;
        std::vector<Light*> lights;

        Scene() = default;

    public:
        void add_object(Object* obj)
        {
            objects.push_back(obj);
        }

        void add_light(Light* light)
        {
            lights.push_back(light);
        }
    };
}
