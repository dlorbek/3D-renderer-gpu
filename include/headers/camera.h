#ifndef CAMERA_H_
#define CAMERA_H_

#include "headers/math3d.h"


class Camera{
    public:
        Vec3 position;
        Vec3 direction;
        float yaw;
        float pitch;
        float speed = 0.05f;
        float sensitivity = 0.05f;

        Camera();
        Mat4 lookAt(const Vec3& pos, const Vec3& target, const Vec3& up) const;
        Mat4 getView() const;
        void updateDirection(void);
        
        
    private:
        



};









#endif