#ifndef MATH3D_H_
#define MATH3D_H_

#include <SDL3/SDL.h>
#include <math.h>

#define PI 3.14159265359
#define EPS 1e-6

struct Vec3
{
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;    

    Vec3() = default;
    Vec3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}

    // basic math
    Vec3 operator+(const Vec3& rhs) const { return { x + rhs.x, y + rhs.y, z + rhs.z }; }
    Vec3 operator-(const Vec3& rhs) const { return { x - rhs.x, y - rhs.y, z - rhs.z }; }
    Vec3 operator*(float s) const { return { x * s, y * s, z * s }; }

    float dot(const Vec3& r) const {
        return x*r.x + y*r.y + z*r.z;
    }

    Vec3 cross(const Vec3& r) const {
        return {
            y * r.z - z * r.y,
            z * r.x - x * r.z,
            x * r.y - y * r.x
        };
    }
};




struct Vec4 {
    float x, y, z, w;

    Vec4() : x(0.0f), y(0.0f), z(0.0f), w(1.0f) {}
    Vec4(float x_, float y_, float z_, float w_)
        : x(x_), y(y_), z(z_), w(w_) {}
};


struct Mat4 {
    float m[16]; // column-major: m[col*4 + row]

    // Identity by default
    Mat4() {
        for (int i = 0; i < 16; ++i)
            m[i] = (i % 5 == 0) ? 1.0f : 0.0f;
    }

    static Mat4 identity() {
        return Mat4();
    }

    inline float& at(int row, int col) {
        return m[col * 4 + row];
    }

    inline const float& at(int row, int col) const {
        return m[col * 4 + row];
    }

    // ---------- Matrix * Matrix
    Mat4 operator*(const Mat4& rhs) const {
        Mat4 r;
        for (int row = 0; row < 4; ++row) {
            for (int col = 0; col < 4; ++col) {
                r.at(row, col) =
                    at(row, 0) * rhs.at(0, col) +
                    at(row, 1) * rhs.at(1, col) +
                    at(row, 2) * rhs.at(2, col) +
                    at(row, 3) * rhs.at(3, col);
            }
        }
        return r;
    }

    // ---------- Matrix * Vec4
    Vec4 operator*(const Vec4& v) const {
        return {
            at(0,0)*v.x + at(0,1)*v.y + at(0,2)*v.z + at(0,3)*v.w,
            at(1,0)*v.x + at(1,1)*v.y + at(1,2)*v.z + at(1,3)*v.w,
            at(2,0)*v.x + at(2,1)*v.y + at(2,2)*v.z + at(2,3)*v.w,
            at(3,0)*v.x + at(3,1)*v.y + at(3,2)*v.z + at(3,3)*v.w
        };
    }

    // ---------- Translation
    static Mat4 translation(const Vec3& t) {
        Mat4 m = identity();
        m.at(0,3) = t.x;
        m.at(1,3) = t.y;
        m.at(2,3) = t.z;
        return m;
    }

    // ---------- Rotation X
    static Mat4 rotationX(float angle) {
        Mat4 m = identity();
        float c = cosf(angle);
        float s = sinf(angle);

        m.at(1,1) = c;
        m.at(1,2) = -s;
        m.at(2,1) = s;
        m.at(2,2) = c;
        return m;
    }

    // ---------- Rotation Y
    static Mat4 rotationY(float angle) {
        Mat4 m = identity();
        float c = cosf(angle);
        float s = sinf(angle);

        m.at(0,0) = c;
        m.at(0,2) = s;
        m.at(2,0) = -s;
        m.at(2,2) = c;
        return m;
    }

    // ---------- Rotation Z
    static Mat4 rotationZ(float angle) {
        Mat4 m = identity();
        float c = cosf(angle);
        float s = sinf(angle);

        m.at(0,0) = c;
        m.at(0,1) = -s;
        m.at(1,0) = s;
        m.at(1,1) = c;
        return m;
    }

    // ----------- Scale
    static Mat4 scale(const Vec3& s)
    {
        Mat4 r = identity();
        r.m[0]  = s.x;
        r.m[5]  = s.y;
        r.m[10] = s.z;
        return r;
    }

    // ---------- Perspective projection
    static Mat4 projection(float fovY, float aspect, float zNear, float zFar) {
        Mat4 m{};
        float f = 1.0f / tanf(fovY * 0.5f);

        m.at(0,0) = f / aspect;
        m.at(1,1) = f;
        m.at(2,2) = (zFar + zNear) / (zNear - zFar);
        m.at(2,3) = (2.0f * zFar * zNear) / (zNear - zFar);
        m.at(3,2) = -1.0f;
        m.at(3,3) = 0.0f;

        return m;
    }
};






struct Mat3
{
    float m[3][3] = { 0 }; // 3x3 matrix

    // Default constructor: identity matrix
    Mat3()
    {
        m[0][0] = 1.0f; m[1][1] = 1.0f; m[2][2] = 1.0f;
    }

    // Constructor from 9 floats
    Mat3(float m00, float m01, float m02,
         float m10, float m11, float m12,
         float m20, float m21, float m22)
    {
        m[0][0]=m00; m[0][1]=m01; m[0][2]=m02;
        m[1][0]=m10; m[1][1]=m11; m[1][2]=m12;
        m[2][0]=m20; m[2][1]=m21; m[2][2]=m22;
    }

    // Multiply with another Mat3
    Mat3 operator*(const Mat3& rhs) const
    {
        Mat3 result;
        for(int i=0; i<3; ++i)
        {
            for(int j=0; j<3; ++j)
            {
                result.m[i][j] = 0.0f;
                for(int k=0; k<3; ++k)
                    result.m[i][j] += m[i][k] * rhs.m[k][j];
            }
        }
        return result;
    }

    // Multiply with Vec3
    Vec3 operator*(const Vec3& v) const
    {
        return {
            m[0][0]*v.x + m[0][1]*v.y + m[0][2]*v.z,
            m[1][0]*v.x + m[1][1]*v.y + m[1][2]*v.z,
            m[2][0]*v.x + m[2][1]*v.y + m[2][2]*v.z
        };
    }
};




Vec3 normalize(const Vec3& v);

Mat3 rotationMatrixX(float angle);
Mat3 rotationMatrixY(float angle);
Mat3 rotationMatrixZ(float angle);
Mat3 scaleMatrix(float s);

float radians(float);
float degrees(float);





#endif