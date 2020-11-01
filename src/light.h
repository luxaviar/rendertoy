#pragma once

#include <vector>
#include <assert.h>
#include "math/vec3.h"
#include "math/mat4.h"
#include "common/uncopyable.h"
#include "rendertexture.h"

namespace rendertoy {

enum class LightType : uint8_t {
    kDirection,
    kPoint,
};

struct Light {
    Vec3f position;
    Vec3f direction;
    Vec3f color;
    float intensity;
    LightType type;
    Matrix4x4 view_matrix;
    Matrix4x4 project_matrix;
    Matrix4x4 vp_matrix;
    Matrix4x4 mvp;
    RenderTexture* shadow_map;

    Light() : type(LightType::kDirection), shadow_map(nullptr) {

    }

    void SetupShadow(float w, float h, float n, float f, RenderTexture* rt);
};

}
