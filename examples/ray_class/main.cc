#include "color.h"
#include "ray.h"
#include "vec3.h"

#include <iostream>

#define ASPECT_RATIO (16.0 / 9.0)
#define IMAGE_WIDTH 400
#define FOCAL_LENGTH 1.0
#define VIEWPORT_HEIGHT 2.0

color ray_color(const ray &r)
{
    vec3 unit_direction = unit_vector(r.direction());
    auto t = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - t) * color(1.0, 1.0, 1.0) + t * color(0.5, 0.7, 1.0);
}

void render(const int image_height, const point3 origin, const point3 lower_left_corner, const point3 horizontal, const point3 vertical)
{
    std::cout << "P3\n"
              << IMAGE_WIDTH << " " << image_height << "\n255\n";

    for (int j = image_height - 1; j >= 0; --j)
    {
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < IMAGE_WIDTH; ++i)
        {
            auto u = double(i) / (IMAGE_WIDTH - 1);
            auto v = double(j) / (image_height - 1);
            ray r(origin, lower_left_corner + u * horizontal + v * vertical - origin);
            color pixel_color = ray_color(r);
            write_color(std::cout, pixel_color);
        }
    }
    std::cerr << "\nDone.\n";
}

int main()
{

    // Image
    const int image_height = static_cast<int>(IMAGE_WIDTH / ASPECT_RATIO);

    // Camera

    float viewport_width = ASPECT_RATIO * VIEWPORT_HEIGHT;

    point3 origin(0, 0, 0);
    point3 horizontal(viewport_width, 0, 0);
    point3 vertical(0, VIEWPORT_HEIGHT, 0);
    point3 lower_left_corner = origin - horizontal / 2 - vertical / 2 - vec3(0, 0, FOCAL_LENGTH);

    // Render
    render(image_height, origin, lower_left_corner, horizontal, vertical);
}