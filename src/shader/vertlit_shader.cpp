#include "vertlit_shader.h"
#include  <cmath>

namespace rendertoy {

VertexOut VertLitShader::Vert(const Vertex& v) const {
    VertexOut v2f;
    v2f.position = uniform_->mvp * v.position;
    v2f.color = v.color;
    v2f.normal = uniform_->model.MultiplyVector(v.normal);
    v2f.texcoord = v.texcoord;
    return v2f;
}

Vec4f VertLitShader::Frag(const VertexOut& v2f) const {
    return v2f.color;
}

}
