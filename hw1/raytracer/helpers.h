#ifndef HELPERS_H
#define HELPERS_H

#include "vec3.h"
#include "ray.h"

static const double EPSILON = 0.000001;
static const double INF = std::numeric_limits<double>::infinity();

static inline double determinant(const vec3 &col1, const vec3 &col2,
                                 const vec3 &col3)
{
    return col1.x * (col2.y * col3.z - col3.y * col2.z) +
           col1.y * (col3.x * col2.z - col3.z * col2.x) +
           col1.z * (col2.x * col3.y - col2.y * col3.x);
}

static bool intersect(const point3 &v0, const point3 &v1, const point3 &v2,
                      const ray &r, const double &t_min, const double &t_max,
                      double &t)
{

    vec3 a_b = v0 - v1;
    vec3 a_c = v0 - v2;
    double detA = determinant(a_b, a_c, r.direction());
    if (detA > -EPSILON && detA < EPSILON)
        return false;

    vec3 a_o = v0 - r.origin();
    double inv_detA = 1.0 / detA;

    double beta = determinant(a_o, a_c, r.direction()) * inv_detA;
    if (beta < 0.0 || beta > 1.0)
        return false;

    double gamma = determinant(a_b, a_o, r.direction()) * inv_detA;
    if (gamma < 0.0 || beta + gamma > 1.0)
        return false;

    t = determinant(a_b, a_c, a_o) * inv_detA;

    return t > t_min && t < t_max;
}

static double max(const double a, const double b)
{
    return a > b ? a : b;
}

static int clamp(const double a)
{
    int x = static_cast<int>(a);
    return max(0, x > 255 ? 255 : x);
}

#endif // HELPERS_H