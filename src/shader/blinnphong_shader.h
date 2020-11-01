#pragma once

#include "shader.h"
#include "common/singleton.h"
#include "math/vec3.h"
#include "light.h"

namespace rendertoy {

//surfaceColor = emissive + ambient + diffuse + specular
class BlinnPhongShader : public Shader, public Singleton<BlinnPhongShader> {
public:
    VertexOut Vert(const Vertex& v) const override;
    Vec4f Frag(const VertexOut& v2f) const override;

private:
    Vec3f CalcLight(const Light& light, const Vec3f& view_dir, 
        float gloss, const VertexOut& v2f, const Vec3f& normal, const Vec3f& albedo) const;

protected:
    BlinnPhongShader() : Shader("BlinnPhong") {}
};

}
