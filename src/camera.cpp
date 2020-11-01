#include "camera.h"
#include  <cmath>

namespace rendertoy {

Camera::Camera(float fov_, float near_, float far_, const Vec3f& pos_, const Vec3f& target_, const Vec3f& up_) : 
    pos(pos_),
    target(target_),
    up(up_),
    fov(fov_ * math::kDeg2Rad),
    aspect(1.0f),
    near(near_),
    far(far_)
{
    
}

Camera::Camera() : Camera(45, 0.1, 50, {0, 0, -5}, Vec3f::zero, Vec3f::up) {
    
}

Matrix4x4 Camera::view_matrix() const {
    Matrix4x4 reverse_z(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, -1, 0,
        0, 0, 0, 1
    );

    return reverse_z * Matrix4x4::LookAt(pos, target, up);
}

Matrix4x4 Camera::projection_matrix() const {
    //float fov = fov * math::kDeg2Rad;
    float half_cot = 1.0f / std::tan(fov / 2.0f);
    float reverse_diff = 1.0f  / (far - near);

    Matrix4x4 projection(
        half_cot / aspect, 0, 0, 0,
        0, half_cot, 0, 0,
        0, 0, -(near + far) * reverse_diff, -2.0f * near * far * reverse_diff,
        0, 0, -1.0f, 0
    );

    return projection;
}

}