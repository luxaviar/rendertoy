#pragma once

#include "shader.h"
#include "common/singleton.h"

namespace rendertoy {

class SkyboxShader : public Shader, public Singleton<SkyboxShader> {
public:
    VertexOut Vert(const Vertex& v) const override;
    Vec4f Frag(const VertexOut& v2f) const override;
    
protected:
    SkyboxShader() : Shader("Skybox") {
        write_depth_ = false;
        cull_ = CullMode::kFront;
    }
};

}
