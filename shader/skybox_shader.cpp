#include "skybox_shader.h"
#include "material/skybox_material.h"
#include "color.h"

namespace rendertoy {

VertexOut SkyboxShader::Vert(const Vertex& v) const {
    VertexOut v2f;
    // do not move box
    v2f.position = uniform_->projection * uniform_->view.MultiplyVector(v.position);
    //Set Z to W guarantee that the final Z value of the position will be 1.0. This Z value is always mapped to the far Z. 
    v2f.position.z = v2f.position.w;
    //save the object position on the box as coord for sample3D, the box center is always placed at the eye origin
    v2f.world_position = Vec3f(v.position.x, v.position.y, v.position.z);// -uniform_->camera_pos;
    return v2f;
}

Vec4f SkyboxShader::Frag(const VertexOut& v2f) const {    
    const SkyboxMaterial* mat = static_cast<const SkyboxMaterial*>(uniform_->mat);
    Vec3f coord = v2f.world_position.Normalize();
    return mat->skybox_tex->Sample3D(coord);
}

}
