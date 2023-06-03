#ifndef VEC3_H
#define VEC3_H

#include <cmath>

class Vec3
{
public:
    float x, y, z;

    Vec3()
        : x(0.0f), y(0.0f), z(0.0f) {}
        
    Vec3(float _x, float _y, float _z)
        : x(_x), y(_y), z(_z) {}

    Vec3 operator+(const Vec3 &other) const
    {
        return Vec3(x + other.x, y + other.y, z + other.z);
    }

    Vec3 &operator+=(const Vec3 &other)
    {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }

    Vec3 &operator-=(const Vec3 &other)
    {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return *this;
    }

    Vec3 operator-(const Vec3 &other) const
    {
        return Vec3(x - other.x, y - other.y, z - other.z);
    }

    Vec3 operator*(float scalar) const
    {
        return Vec3(x * scalar, y * scalar, z * scalar);
    }

    float length() const
    {
        return std::sqrt(x * x + y * y + z * z);
    }

    Vec3 normalize() const
    {
        float len = length();
        return Vec3(x / len, y / len, z / len);
    }

    float dot(const Vec3 &other) const
    {
        return x * other.x + y * other.y + z * other.z;
    }

    Vec3 cross(const Vec3 &other) const
    {
        return Vec3(y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x);
    }

    Vec3 operator-() const
    {
        return Vec3(-x, -y, -z);
    }
};

// clamp
template <typename T>
T clamp(T value, T min, T max)
{
    if (value < min)
        return min;
    if (value > max)
        return max;
    return value;
}

#endif // VEC3_H