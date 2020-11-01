#include "vertlit_material.h"
#include "shader/vertlit_shader.h"

namespace rendertoy {

VertLitMaterial::VertLitMaterial() : Material("VertLit") {
    Shader* shader = VertLitShader::Instance();
    AddPass(shader);
}

}
