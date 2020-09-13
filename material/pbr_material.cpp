#include "pbr_material.h"
#include "shader/pbr_shader.h"

namespace rendertoy {

PbrMaterial::PbrMaterial() : 
    Material("PBR"), 
    metallic(0.4f),
    roughness(0.4f),
    // ka(0.0f),
    ambient_color(0.0f), 
    f0(0.04f),
    albedo_tex(nullptr),
    normal_tex(nullptr),
    metallic_tex(nullptr),
    roughness_tex(nullptr),
    ao_tex(nullptr),
    irradiance_tex(nullptr),
    radiance_tex(nullptr),
    brdf_lut(nullptr)
{
    Shader* shader = PbrShader::Instance();
    AddPass(shader);
}

}
