#pragma once

#include "shader.h"
#include "singleton.h"

namespace rendertoy {

class VertLitShader : public Shader, public Singleton<VertLitShader> {
public:
    VertexOut Vert(const Vertex& v) const override;
    Vec4f Frag(const VertexOut& v2f) const override;
    
protected:
    VertLitShader() : Shader("VertLit") { }
};

}
