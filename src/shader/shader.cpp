#include "vertlit_shader.h"
#include  <cmath>

namespace rendertoy {

void Shader::SetShadowCoord(const Vertex& v, VertexOut& v2f) const {
    if (uniform_->shadow_light_) {
        Vec4f light_pos = uniform_->shadow_light_->mvp * v.position;
        light_pos /= light_pos.w;
        light_pos = light_pos * 0.5f + 0.5f;
        v2f.shadow_coord = { light_pos.x, light_pos.y, light_pos.z };
    }
}

bool Shader::IsInShadow(const Light& light, const VertexOut& v2f, float ndotl) const {    
    if (&light == uniform_->shadow_light_) {
        float depth = uniform_->shadow_light_->shadow_map->depth_buffer().Sample(v2f.shadow_coord.x, v2f.shadow_coord.y);
        float depth_bias = math::Max(0.05f * (1 - ndotl), 0.005f);
        if (v2f.shadow_coord.z - depth_bias > depth) { //current depth > depth
            return true;
        }
    }
    return false;
}

}
