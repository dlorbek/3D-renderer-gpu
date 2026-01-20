#include "headers/math3d.h"




Vec3 normalize(const Vec3& v){
    return v * (1 / sqrtf(v.x*v.x + v.y*v.y + v.z*v.z));
}



Mat3 rotationMatrixX(float angle)
{
    float c = cosf(angle);
    float s = sinf(angle);
    return Mat3(
        1, 0, 0,
        0, c, -s,
        0, s, c
    );
}

Mat3 rotationMatrixY(float angle)
{
    float c = cosf(angle);
    float s = sinf(angle);
    return Mat3(
        c, 0, s,
        0, 1, 0,
        -s, 0, c
    );
}

Mat3 rotationMatrixZ(float angle)
{
    float c = cosf(angle);
    float s = sinf(angle);
    return Mat3(
        c, -s, 0,
        s,  c, 0,
        0,  0, 1
    );
}

Mat3 scaleMatrix(float s)
{
    return Mat3(
        s, 0, 0,
        0, s, 0,
        0, 0, s
    );
}

float radians(float deg){
    return deg * PI / 180.0f;
}
float degrees(float rad){
    return rad * 180.0f / PI;
}



