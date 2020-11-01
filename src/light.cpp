#include "light.h"
#include <assert.h>

namespace rendertoy {

void Light::SetupShadow(float w, float h, float n, float f, RenderTexture* rt) {
    assert(type == LightType::kDirection);
    Matrix4x4 reverse_z(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, -1, 0,
        0, 0, 0, 1
    );

    view_matrix = reverse_z * Matrix4x4::LookAt(Vec3f::zero, direction, Vec3f::up);
    project_matrix = Matrix4x4(
        2.0f / w, 0, 0, 0,
        0, 2.0f / h, 0, 0,
        0, 0, -2.0f / (f - n), -(f + n) / (f - n),
        0, 0, 0, 1
    );

    vp_matrix = project_matrix * view_matrix;

    shadow_map = rt;
}

}
