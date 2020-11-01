#include "normal_material.h"
#include "shader/normal_shader.h"

namespace rendertoy {

NormalMaterial::NormalMaterial() : Material("Normal") {
    Shader* shader = NormalShader::Instance();
    AddPass(shader);
}

}
