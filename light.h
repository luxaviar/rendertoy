#pragma once

#include <vector>
#include <assert.h>
#include "math/vec3.h"
#include "math/mat4.h"
#include "uncopyable.h"

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
};

}
