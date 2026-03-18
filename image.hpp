#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "color.hpp"

namespace RayTracer
{
    class Image
    {
    public:
        const unsigned width;
        const unsigned height;
        Color* pixels;

        Image(unsigned width, unsigned height): width(width), height(height)
        {
            pixels = new Color[width * height];
        };

        ~Image()
        {
            delete[] pixels;
        }

    public:
        void set_pixel(unsigned x, unsigned y, Color color)
        {
            pixels[width * y + x] = color;
        }

        void to_ppm(const char* path)
        {
            auto file = std::ofstream(path);

            file << "P3\n" << width << " " << height << "\n255\n";

            for (int j = 0; j < height; j++)
            {
                for (int i = 0; i < width; i++)
                {
                    auto c = pixels[width * j + i];
                    auto r = static_cast<int>(c.r * 255.999);
                    auto g = static_cast<int>(c.g * 255.999);
                    auto b = static_cast<int>(c.b * 255.999);

                    file << r << " " << g << " " << b << "\n";
                }
            }
        }
    };
}
