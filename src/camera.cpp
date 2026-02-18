#include "headers/camera.h"




Camera::Camera(){
    position = {0.0f, 0.0f, 2.0f};
    yaw = -90.0f;
    pitch = 0.0f;
    updateDirection();
}




Mat4 Camera::lookAt(const Vec3& pos, const Vec3& target, const Vec3& up) const
{
    Vec3 f = normalize(target - pos);
    Vec3 r = normalize(f.cross(up));
    Vec3 u = r.cross(f);

    Mat4 result;

    result.m[0] = r.x;
    result.m[1] = u.x;
    result.m[2] = -f.x;

    result.m[4] = r.y;
    result.m[5] = u.y;
    result.m[6] = -f.y;

    result.m[8]  = r.z;
    result.m[9]  = u.z;
    result.m[10] = -f.z;

    result.m[12] = -r.dot(pos);
    result.m[13] = -u.dot(pos);
    result.m[14] =  f.dot(pos);


    return result;
}


Mat4 Camera::getView() const
{
    return lookAt(position, position + direction, Vec3{0,1,0});
}



void Camera::updateDirection(void)
{
    direction.x = cos(radians(yaw)) * cos(radians(pitch));
    direction.y = sin(radians(pitch));
    direction.z = sin(radians(yaw)) * cos(radians(pitch));
    direction = normalize(direction);
}