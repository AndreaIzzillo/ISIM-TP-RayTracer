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
        unsigned width;
        unsigned height;
        std::vector<Color> pixels;

        Image() = default;
        Image(const char *path)
        {
            auto file = std::ifstream(path);
            std::string token;

            file >> token; // PPM type
            file >> token; // Width
            width = std::stoi(token);
            file >> token; // Height
            height = std::stoi(token);
            file >> token; // RGB

            pixels = std::vector<Color>(width * height);

            unsigned i = 0;
            std::string r, g, b;
            while (file >> r && file >> g && file >> b)
            {
                pixels[i].r = std::stod(r) / 255.0;
                pixels[i].g = std::stod(g) / 255.0;
                pixels[i].b = std::stod(b) / 255.0;
                i++;
            }
        };
        Image(unsigned width, unsigned height): width(width), height(height)
        {
            pixels = std::vector<Color>(width * height);
        };

    public:
        void set_pixel(unsigned x, unsigned y, Color color)
        {
            pixels[width * y + x] = color;
        }

        Color get_pixel(unsigned x, unsigned y)
        {
            return pixels[width * y + x];
        }

        void to_ppm(const char* path)
        {
            auto file = std::ofstream(path);

            file << "P3\n" << width << " " << height << "\n255\n";

            for (int j = 0; j < height; j++)
            {
                for (int i = 0; i < width; i++)
                {
                    auto c = pixels[width * j + i].clamp();
                    auto r = static_cast<int>(c.r * 255.999);
                    auto g = static_cast<int>(c.g * 255.999);
                    auto b = static_cast<int>(c.b * 255.999);

                    file << r << " " << g << " " << b << "\n";
                }
            }
        }
    };
}
