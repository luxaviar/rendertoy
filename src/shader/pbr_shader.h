#pragma once

#include "shader.h"
#include "common/singleton.h"

namespace rendertoy {

class PbrShader : public Shader, public Singleton<PbrShader> {
public:
    VertexOut Vert(const Vertex& v) const override;
    Vec4f Frag(const VertexOut& v2f) const override;
private:
    Vec3f CalcLight(const Light& light, const VertexOut& v2f, const Vec3f& view_dir, const Vec3f& normal, const Vec3f& albedo,
        const Vec3f& f0, float roughness, float metallic) const;

    Vec3f EvaluateIBL(const Vec3f& view_dir, const Vec3f& normal, const Vec3f& f0,
        const Vec3f& albedo, float metallic, float roughness, float ao) const;

protected:
    PbrShader() : Shader("PBR") {}
};

}
