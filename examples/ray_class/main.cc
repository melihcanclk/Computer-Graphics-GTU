#include "color.h"
#include "ray.h"
#include "vec3.h"

#include <iostream>

#define ASPECT_RATIO (16.0 / 9.0)
#define IMAGE_WIDTH 400
#define FOCAL_LENGTH 1.0
#define VIEWPORT_HEIGHT 2.0

bool hit_sphere(const point3 &center, float radius, const ray &r)
{
    // ray-sphere intersection
    // a = B dot B
    // b = 2B dot (A - C)
    // c = (A - C) dot (A - C) - R^2
    // discriminant = b^2 - 4ac
    // if discriminant < 0, no intersection
    // if discriminant = 0, one intersection
    // if discriminant > 0, two intersections
    // if t > 0, the intersection is in front of the camera
    // if t < 0, the intersection is behind the camera
    // if t = 0, the intersection is on the camera
    vec3 origin_to_center = r.origin() - center;
    float a = dot(r.direction(), r.direction());
    float b = 2.0 * dot(origin_to_center, r.direction());
    float c = dot(origin_to_center, origin_to_center) - radius * radius;
    float discriminant = b * b - 4 * a * c;
    return (discriminant > 0);
}

color ray_color(const ray &r)
{
    // if we hit the sphere, return the color of the sphere
    point3 sphere_center(0, 0, -2);
    float sphere_radius = 0.5;
    if (hit_sphere(sphere_center, sphere_radius, r))
    {
        return color(0, 1, 0);
    }
    else
    {
        vec3 unit_direction = unit_vector(r.direction());
        float t = 0.5 * (unit_direction.y() + 1.0);
        // return background color
        return (1.0 - t) * color(1.0, 1.0, 1.0) + t * color(0.5, 0.7, 1.0);
    }
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