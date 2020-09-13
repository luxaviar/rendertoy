#pragma once

#include <vector>
#include <assert.h>
#include "math/vec3.h"
#include "math/mat4.h"
#include "uncopyable.h"

namespace rendertoy {

struct Camera {
    Camera(float fov_, float near_, float far_, const Vec3f& pos_, const Vec3f& target_, const Vec3f& up_);
    Camera();

    Matrix4x4 view_matrix() const;
    Matrix4x4 projection_matrix() const;

    Vec3f pos;
    Vec3f target;
    Vec3f up;

    float fov;
    float aspect;
    float near;
    float far;
};

}
