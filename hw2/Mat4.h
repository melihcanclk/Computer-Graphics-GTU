#ifndef MAT4_H
#define MAT4_H

#include "Vec3.h" // Assuming Vec3 class implementation is in "Vec3.h" header file

class Mat4
{
public:
    float data[16];

    Mat4()
    {
        for (int i = 0; i < 16; ++i)
            data[i] = 0.0f;
    }

    static Mat4 identity()
    {
        Mat4 result;
        result.data[0] = 1.0f;
        result.data[5] = 1.0f;
        result.data[10] = 1.0f;
        result.data[15] = 1.0f;
        return result;
    }

    Mat4 operator*(const Mat4 &other) const
    {
        Mat4 result;

        for (int row = 0; row < 4; ++row)
        {
            for (int col = 0; col < 4; ++col)
            {
                for (int k = 0; k < 4; ++k)
                {
                    result.data[row * 4 + col] += data[row * 4 + k] * other.data[k * 4 + col];
                }
            }
        }

        return result;
    }

    Mat4 &operator*=(const Mat4 &other)
    {
        *this = *this * other;
        return *this;
    }

    static Mat4 translate(const Vec3 &translation)
    {
        Mat4 result = identity();
        result.data[12] = translation.x;
        result.data[13] = translation.y;
        result.data[14] = translation.z;
        return result;
    }

    static Mat4 rotate(float angle, const Vec3 &axis)
    {
        Mat4 result;
        float radians = angle * 3.14159f / 180.0f;
        float c = std::cos(radians);
        float s = std::sin(radians);
        float omc = 1.0f - c;

        float x = axis.x;
        float y = axis.y;
        float z = axis.z;

        result.data[0] = x * x * omc + c;
        result.data[1] = x * y * omc + z * s;
        result.data[2] = x * z * omc - y * s;

        result.data[4] = x * y * omc - z * s;
        result.data[5] = y * y * omc + c;
        result.data[6] = y * z * omc + x * s;

        result.data[8] = x * z * omc + y * s;
        result.data[9] = y * z * omc - x * s;
        result.data[10] = z * z * omc + c;

        result.data[15] = 1.0f;

        return result;
    }

    static Mat4 scale(const Vec3 &scale)
    {
        Mat4 result = identity();
        result.data[0] = scale.x;
        result.data[5] = scale.y;
        result.data[10] = scale.z;
        return result;
    }

    static Mat4 lookAt(const Vec3 &eye, const Vec3 &target, const Vec3 &up)
    {
        Vec3 cameraFront = (target - eye).normalize();
        Vec3 cameraRight = up.cross(cameraFront).normalize();
        Vec3 cameraUp = cameraFront.cross(cameraRight).normalize();

        Mat4 result;
        result.data[0] = cameraRight.x;
        result.data[4] = cameraRight.y;
        result.data[8] = cameraRight.z;
        result.data[1] = cameraUp.x;
        result.data[5] = cameraUp.y;
        result.data[9] = cameraUp.z;
        result.data[2] = -cameraFront.x;
        result.data[6] = -cameraFront.y;
        result.data[10] = -cameraFront.z;
        result.data[15] = 1.0f;

        return result * translate(-eye);
    }
};

#endif // MAT4_H