#include "material.h"
#include  <cmath>

namespace rendertoy {

void Material::AddPass(Shader* shader) {
    pass_.push_back(shader);
}

}