#pragma once

#include "material.h"
#include "texture3D.h"

namespace rendertoy {

class SkyboxMaterial : public Material {
public:
    SkyboxMaterial();

    Texture3D* skybox_tex;
};

}
