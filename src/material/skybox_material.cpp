#include "skybox_material.h"
#include "shader/skybox_shader.h"

namespace rendertoy {

SkyboxMaterial::SkyboxMaterial() : Material("SkyBox"), skybox_tex(nullptr) {
    Shader* shader = SkyboxShader::Instance();
    AddPass(shader);
}

}
