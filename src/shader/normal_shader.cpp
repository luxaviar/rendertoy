#include "normal_shader.h"
#include  <cmath>

namespace rendertoy {

VertexOut NormalShader::Vert(const Vertex& v) const {
    VertexOut v2f;
    v2f.position = uniform_->mvp * v.position;
    v2f.color = v.color;
    v2f.normal = (uniform_->view * uniform_->model).MultiplyVector(v.normal);
    v2f.texcoord = v.texcoord;
    return v2f;
}

Vec4f NormalShader::Frag(const VertexOut& v2f) const {
    Vec3f color = (v2f.normal.Normalize() + Vec3f::one) / 2;
    return { color, 1.0f };
}

}
