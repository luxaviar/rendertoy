#pragma once

#include <vector>
#include "uncopyable.h"
#include "math/vec2.h"
#include "math/vec3.h"
#include "math/vec4.h"
#include "math/mat4.h"
#include "light.h"

namespace  rendertoy
{

class Material;

struct Uniform {
    Matrix4x4 model;
    Matrix4x4 view;
    Matrix4x4 projection;
    Matrix4x4 mvp;
    Matrix4x4 vp;

    Vec3f camera_pos;
    std::vector<Light> lights;
    const Material* mat;
};

}
