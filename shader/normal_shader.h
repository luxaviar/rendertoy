#pragma once

#include "shader.h"
#include "singleton.h"

namespace rendertoy {

class NormalShader : public Shader, public Singleton<NormalShader> {
public:
    VertexOut Vert(const Vertex& v) const override;
    Vec4f Frag(const VertexOut& v2f) const override;
    
protected:
    NormalShader() : Shader("Normal") {}
};

}
