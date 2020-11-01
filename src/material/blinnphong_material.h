#pragma once

#include "material.h"
#include "texture2D.h"

namespace rendertoy {

class BlinnPhongMaterial : public Material {
public:
    BlinnPhongMaterial();
 
    Vec3f ke;
    Vec3f ka;
    //Vec3f kd; //use albedo
    //Vec3f ks; //use albedo
    float gloss;
    Vec3f ambient_color;
    Texture2D* main_tex;
    Texture2D* normal_tex;
};

}
