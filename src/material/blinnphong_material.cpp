#include "blinnphong_material.h"
#include "shader/blinnphong_shader.h"

namespace rendertoy {

BlinnPhongMaterial::BlinnPhongMaterial() : 
    Material("BlinnPhong"), 
    ke(0.0f),
    ka(0.0f),
    //kd(0.0f),
    //ks(0.0f),
    gloss(100.0f),
    ambient_color(Vec3f::zero), 
    main_tex(nullptr),
    normal_tex(nullptr)
{
    Shader* shader = BlinnPhongShader::Instance();
    AddPass(shader);
}

}
