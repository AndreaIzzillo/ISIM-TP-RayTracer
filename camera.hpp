#pragma once

#include <cmath>

#include "geometry.hpp"
#include "ray.hpp"

namespace RayTracer
{
    class Camera
    {
    public:
        Point3 C;
        Point3 P;

        /* Camera basis */
        Vector3 front;
        Vector3 right;
        Vector3 up;

        /* Viewport definition */
        double alpha;
        double beta;
        double d;

        double width;
        double height;
        double aspect_ratio;

        Point3 viewport;

        /* Pixels */
        unsigned w_res;
        unsigned h_res;

        double pixel_width;
        double pixel_height;

        Vector3 w_step;
        Vector3 h_step;

        Camera(Point3 center, Point3 point_at,
               double horizontal_fov, double vertical_fov, double focal_dist,
               unsigned horizontal_resolution)
            : C(center), P(point_at),
              alpha(horizontal_fov), beta(vertical_fov), d(focal_dist)
        {
            front = (P - C).normalize();
            right = (front ^ Vector3(0.0, 0.0, 1.0)).normalize();
            up = (right ^ front).normalize();

            width = 2.0 * d * tan(alpha / 2.0);
            height = 2.0 * d * tan(beta / 2.0);

            aspect_ratio = width / height;

            w_res = horizontal_resolution;
            h_res = static_cast<unsigned>(w_res / aspect_ratio);

            pixel_width = width / w_res;
            pixel_height = height / h_res;

            w_step = right * pixel_width;
            h_step = (-up) * pixel_height;

            viewport = C + front * d + up * (height / 2) - right * (width / 2);
        }

    public:
        Ray get_ray(unsigned x, unsigned y)
        {
            auto p = viewport;

            p = p + right * pixel_width / 2;
            p = p + (-up) * pixel_height / 2;

            p = p + w_step * x;
            p = p + h_step * y;

            return {C, p - C};
        }
    };
}
