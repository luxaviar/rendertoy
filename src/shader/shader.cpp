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

float Shader::CalcShadow(const Light& light, const VertexOut& v2f, float ndotl) const {
    if (&light == uniform_->shadow_light_) {
        auto& shadow_coord = v2f.shadow_coord;
        if (shadow_coord.z < 0.0f || shadow_coord.z > 1.0f) {
            return 1.0f;
        }

        auto& shadow_map = uniform_->shadow_light_->shadow_map->depth_buffer();

        static constexpr int pcf_range = 2;

        float result = 0.0f;
        auto& tex_size = shadow_map.tex_size();
        for (int i = -pcf_range; i < pcf_range; ++i) {
            for (int j = -pcf_range; j < pcf_range; ++j) {
                float depth = shadow_map.Sample(shadow_coord.x + tex_size.x * i, shadow_coord.y + tex_size.y * j);
                float depth_bias = math::Max(0.05f * (1 - ndotl), 0.005f);
                if (shadow_coord.z - depth_bias < depth) { //current depth < light depth -> in front of light depth -> not in shadow
                    result += 1.0f;
                }
            }
        }

        return result / (pcf_range * 2 + 1);
    }

    return 1.0f;
}

}
