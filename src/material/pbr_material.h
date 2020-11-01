#pragma once

#include "material.h"
#include "texture3D.h"

namespace rendertoy {

class PbrMaterial : public Material {
public:
    PbrMaterial();
 
    float metallic;
    float roughness;
    // Vec3f ka;
    Vec3f ambient_color;
    Vec3f f0;
    Texture2D* albedo_tex;
    Texture2D* normal_tex;    
    Texture2D* metalroughness_tex;
    Texture2D* ao_tex;
    Texture2D* emission_tex;
    Texture3D* irradiance_tex;
    Texture3D* radiance_tex;
    Texture2D* brdf_lut;
};

}
