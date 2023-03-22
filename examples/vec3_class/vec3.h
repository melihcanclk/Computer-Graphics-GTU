#ifndef VEC3_H
#define VEC3_H

#include <cmath>
#include <iostream>

using std::sqrt;

class vec3
{
public:
    // default constructor
    vec3() : point{0, 0, 0} {}
    // constructor
    vec3(float e0, float e1, float e2) : point{e0, e1, e2} {}

    float x() const { return point[0]; }
    float y() const { return point[1]; }
    float z() const { return point[2]; }

    vec3 operator-() const { return vec3(-point[0], -point[1], -point[2]); }
    float operator[](int i) const { return point[i]; }
    float &operator[](int i) { return point[i]; }

    vec3 &operator+=(const vec3 &v)
    {
        point[0] += v.point[0];
        point[1] += v.point[1];
        point[2] += v.point[2];
        return *this;
    }

    vec3 &operator*=(const float t)
    {
        point[0] *= t;
        point[1] *= t;
        point[2] *= t;
        return *this;
    }

    vec3 &operator/=(const float t)
    {
        return *this *= 1 / t;
    }

    float length() const
    {
        return sqrt(length_squared());
    }

    float length_squared() const
    {
        return point[0] * point[0] + point[1] * point[1] + point[2] * point[2];
    }

public:
    float point[3];
};

inline std::ostream &operator<<(std::ostream &out, const vec3 &v)
{
    return out << v.point[0] << ' ' << v.point[1] << ' ' << v.point[2];
}

inline vec3 operator+(const vec3 &u, const vec3 &v)
{
    return vec3(u.point[0] + v.point[0], u.point[1] + v.point[1], u.point[2] + v.point[2]);
}

inline vec3 operator-(const vec3 &u, const vec3 &v)
{
    return vec3(u.point[0] - v.point[0], u.point[1] - v.point[1], u.point[2] - v.point[2]);
}

inline vec3 operator*(const vec3 &u, const vec3 &v)
{
    return vec3(u.point[0] * v.point[0], u.point[1] * v.point[1], u.point[2] * v.point[2]);
}

inline vec3 operator*(float t, const vec3 &v)
{
    return vec3(t * v.point[0], t * v.point[1], t * v.point[2]);
}

inline vec3 operator*(const vec3 &v, float t)
{
    return t * v;
}

inline vec3 operator/(vec3 v, float t)
{
    return (1 / t) * v;
}

inline float dot(const vec3 &u, const vec3 &v)
{
    return u.point[0] * v.point[0] + u.point[1] * v.point[1] + u.point[2] * v.point[2];
}

inline vec3 unit_vector(vec3 v)
{
    return v / v.length();
}

// Type aliases for vec3
using point3 = vec3; // 3D point
using color = vec3;  // RGB color

#endif