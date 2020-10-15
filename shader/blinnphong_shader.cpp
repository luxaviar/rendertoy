#include "blinnphong_shader.h"
#include <cmath>
#include "material/blinnphong_material.h"
#include "light.h"
#include "color.h"

namespace rendertoy {

VertexOut BlinnPhongShader::Vert(const Vertex& v) const {
    VertexOut v2f;
    v2f.position = uniform_->mvp * v.position;
    v2f.world_position = uniform_->model.MultiplyPoint3X4({ v.position.x, v.position.y, v.position.z });
    v2f.color = v.color;
    v2f.normal = uniform_->model.MultiplyVector(v.normal); //to world space, make sure there's no non-uniform scale
    v2f.tangent = uniform_->model.MultiplyVector(v.tangent);
    v2f.texcoord = v.texcoord;
    return v2f;
}

Vec4f BlinnPhongShader::Frag(const VertexOut& v2f) const {
    const BlinnPhongMaterial* mat = static_cast<const BlinnPhongMaterial*>(uniform_->mat);
    Vec3f albedo = mat->main_tex->SampleRGB(v2f.texcoord);
    
    Vec3f normal;
    if (mat->normal_tex) {
        Matrix3x3 TBN = v2f.TBN();
        Vec4f tangent_normal = (mat->normal_tex->Sample2D(v2f.texcoord) * 2.0f - 1.0f);
        normal = TBN * Vec3f(tangent_normal.x, tangent_normal.y, tangent_normal.z);
    } else {
        normal = v2f.normal.Normalize();
    }

    Vec3f color = mat->ambient_color * mat->ka; //ambient
    Vec3f view_dir = (uniform_->camera_pos - v2f.world_position).Normalize();

    for (auto& light : uniform_->lights) {
        color += CalcLight(light, view_dir, mat->gloss, v2f, normal, albedo);
    }

    return color;
}

Vec3f BlinnPhongShader::CalcLight(const Light& light, const Vec3f& view_dir, float gloss, const VertexOut& v2f, const Vec3f& normal, const Vec3f& albedo) const {
    Vec3f light_color = light.color * light.intensity;
    Vec3f light_dir = -light.direction;

    if (light.type == LightType::kPoint) {
        float r2 = (light.position - v2f.world_position).MagnitudeSq();
        light_color /= r2; // light attenuation
        light_dir = (light.position - v2f.world_position).Normalize();
    }

    float ndotl = math::Clamp(normal.Dot(light_dir), 0.0f, 1.0f);
    Vec3f color = light_color * albedo * ndotl; //diffuse
    if (ndotl > 0.0f) {
        Vec3f h = (light_dir + view_dir).Normalize();
        float ndoth = math::Clamp(normal.Dot(h), 0.0f, 1.0f);
        color += light_color * albedo * std::pow(ndoth, gloss); //specular
    }

    return color;
}

}
