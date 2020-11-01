#pragma once

#include "shader.h"
#include "common/singleton.h"

namespace rendertoy {

class ShadowShader : public Shader, public Singleton<ShadowShader> {
public:
    VertexOut Vert(const Vertex& v) const override;
    Vec4f Frag(const VertexOut& v2f) const override;
    
protected:
    ShadowShader() : Shader("Shadow") { 
        write_color_ = false;
        cull_ = CullMode::kFront;
    }
};

}
