#include "pbr_shader.h"
#include <cmath>
#include "material/pbr_material.h"
#include "light.h"
#include "color.h"

namespace rendertoy {

inline Vec3f ACESToneMapping(Vec3f color, float adapted_lum=1.0f) {
    constexpr float A = 2.51f;
    constexpr float B = 0.03f;
    constexpr float C = 2.43f;
    constexpr float D = 0.59f;
    constexpr float E = 0.14f;

    color *= adapted_lum;
    return (color * (A * color + B)) / (color * (C * color + D) + E);
}

inline float Fd_Lambert() {
    return 1.0 / math::kPI;
}

inline float F_Schlick(float VoH, float f0, float f90) {
    return f0 + (f90 - f0) * std::pow(1.0 - VoH, 5.0);
}

inline float Fd_Burley(float NoV, float NoL, float LoH, float roughness) {
    float f90 = 0.5 + 2.0 * roughness * LoH * LoH;
    float lightScatter = F_Schlick(NoL, 1.0f, f90);
    float viewScatter = F_Schlick(NoV, 1.0f, f90);
    return lightScatter * viewScatter * (1.0f / math::kPI);
}

inline float D_GGX(float roughness, float NoH, const Vec3f& n, const Vec3f& h) {
    float oneMinusNoHSquared = 1.0 - NoH * NoH;    
    float a = NoH * roughness;
    float k = roughness / (oneMinusNoHSquared + a * a);
    return  k * k * (1.0 / math::kPI);
}

inline Vec3f F_Schlick(const Vec3f& f0, float VoH) {
    float f = std::pow(1.0 - VoH, 5.0);
    return f0 * (1.0 - f) + f; //f0 + (1.0f - f0) * f;
}

inline Vec3f F_Schlick(const Vec3f& f0, float f90, float VoH) {
    // Schlick 1994, "An Inexpensive BRDF Model for Physically-Based Rendering"
    return f0 + (Vec3f(f90) - f0) * std::pow(1.0f - VoH, 5);
}

inline float V_SmithGGXCorrelated(float NoV, float NoL, float roughness) {
    float a2 = roughness * roughness;
    float GGXV = NoL * std::sqrt(NoV * NoV * (1.0f - a2) + a2);
    float GGXL = NoV * std::sqrt(NoL * NoL * (1.0f - a2) + a2);
    return 0.5f / (GGXV + GGXL);
}

float V_SmithGGXCorrelatedFast(float NoV, float NoL, float roughness) {
    float a = roughness;
    float GGXV = NoL * (NoV * (1.0f - a) + a);
    float GGXL = NoV * (NoL * (1.0f - a) + a);
    return 0.5f / (GGXV + GGXL);
}

inline float GeometrySchlickGGX(float NoV, float k) {
    float nom   = NoV;
    float denom = NoV * (1.0 - k) + k;
	
    return nom / denom;
}
  
inline float GeometrySmith(float NoL, float NoV, float k) {
    float ggx1 = GeometrySchlickGGX(NoV, k); // 视线方向的几何遮挡
    float ggx2 = GeometrySchlickGGX(NoL, k); // 光线方向的几何阴影
	
    return ggx1 * ggx2;
}

inline Vec3f PrefilteredDFG(float NoV, float roughness) {
    // 基于Lazarov的Karis逼近
    constexpr Vec4f c0 = Vec4f(-1.0, -0.0275, -0.572,  0.022);
    constexpr Vec4f c1 = Vec4f( 1.0,  0.0425,  1.040, -0.040);
    Vec4f r = c0 * roughness + c1;
    float a004 = (math::Min)(r.x * r.x, std::exp2(-9.28 * NoV)) * r.x + r.y;
    return Vec3f(-1.04f * a004 + r.z, 1.04f * a004 + r.w, 0.0f);
    // 基于Karis的Zioma逼近
    // return vec2(1.0, pow(1.0 - max(roughness, NoV), 3.0));
}

Vec3f PbrShader::CalcLight(const Light& light, const VertexOut& v2f, const Vec3f& view_dir, const Vec3f& normal, const Vec3f& albedo,
        const Vec3f& f0, float roughness, float metallic) const {
    Vec3f radiance = light.color * light.intensity;
    Vec3f light_dir = -light.direction;

    if (light.type == LightType::kPoint) {
        float r2 = (light.position - v2f.world_position).MagnitudeSq();
        radiance /= r2; // light attenuation
        light_dir = (light.position - v2f.world_position).Normalize();
    }

    Vec3f h = (light_dir + view_dir).Normalize();
    float NoL = math::Clamp(normal.Dot(light_dir), 0.0f, 1.0f);
    float NoH = math::Clamp(normal.Dot(h), 0.0f, 1.0f);
    float NoV = math::Clamp(normal.Dot(view_dir), 0.0f, 1.0f);
    float VoH = math::Clamp(view_dir.Dot(h), 0.0f, 1.0f);

    Vec3f diffuse = albedo * Fd_Lambert(); //diffuse
    //Vec3f diffuse = albedo * Fd_Burley(NoV, NoL, math::Clamp(light_dir.Dot(h), 0.0f, 1.0f), roughness); //diffuse

    float D = D_GGX(roughness, NoH, normal, h);
    Vec3f F = F_Schlick(f0, VoH);
    
    float k = std::pow(roughness + 1.0f, 2) / 8.0f;
    float G = GeometrySmith(NoL, NoV, k);
    //float G = V_SmithGGXCorrelatedFast(NoV, NoL, roughness);

    float denominator = 4.0 * NoV * NoL + 0.001f;
    Vec3f specular = (F * D * G) / denominator;
    
    // ks is equal to Fresnel
    Vec3f ks = F;
    // for energy conservation, the diffuse and specular light can't
    // be above 1.0 (unless the surface emits light); to preserve this
    // relationship the diffuse component (kd) should equal 1.0 - ks.
    Vec3f kd = Vec3f(1.0) - ks;
    // multiply kd by the inverse metalness such that only non-metals 
    // have diffuse lighting, or a linear blend if partly metal (pure metals
    // have no diffuse light).
    kd *= (1.0 - metallic);

    // note that we already multiplied the BRDF by the Fresnel (ks) so we won't multiply by ks again
    Vec3f color = (kd * diffuse + specular) * radiance * NoL;
    return color;
}

Vec3f PbrShader::EvaluateIBL(const Vec3f& view_dir, const Vec3f& normal, const Vec3f& f0, 
    const Vec3f& albedo, float metallic, float roughness, float ao) const
{
    const PbrMaterial* mat = static_cast<const PbrMaterial*>(uniform_->mat);

    Vec3f irradiance(0.0f);
    if (mat->irradiance_tex) {
        irradiance = mat->irradiance_tex->SampleRGB(normal);
    }

    Vec3f radiance(0.0f);
    if (mat->radiance_tex) {
        Vec3f reflect = Vec3f::Reflect(-view_dir, normal).Normalize();
        radiance = mat->radiance_tex->SampleRGB(reflect);
    }

    float VoN = math::Clamp(view_dir.Dot(normal), 0.0f, 1.0f);
    Vec3f lut = mat->brdf_lut->SampleRGB(Vec2f(VoN, roughness));
    //Vec3f lut = PrefilteredDFG(VoN, roughness);

    Vec3f F = F_Schlick(f0, VoN);
    Vec3f ks = F;
    Vec3f kd = Vec3f::one - ks;
    kd *= (1.0 - metallic);

    Vec3f indirect_diffuse = irradiance * albedo * Fd_Lambert();
    Vec3f indirect_specular = radiance * (f0 * lut.x + lut.y);

    Vec3f ambient = (kd * indirect_diffuse + indirect_specular) * ao;
    return ambient;
}

VertexOut PbrShader::Vert(const Vertex& v) const {
    VertexOut v2f;
    v2f.position = uniform_->mvp * v.position;
    v2f.world_position = uniform_->model.MultiplyPoint3X4({ v.position.x, v.position.y, v.position.z });
    v2f.color = v.color;
    v2f.normal = uniform_->model.MultiplyVector(v.normal); //to world space, make sure there's no non-uniform scale
    v2f.tangent = uniform_->model.MultiplyVector(v.tangent);
    v2f.texcoord = v.texcoord;
    return v2f;
}

Vec4f PbrShader::Frag(const VertexOut& v2f) const {
    const PbrMaterial* mat = static_cast<const PbrMaterial*>(uniform_->mat);
    Vec3f albedo = mat->albedo_tex->SampleRGB(v2f.texcoord);
    
    Vec3f normal;
    if (mat->normal_tex) {
        Matrix3x3 TBN = v2f.TBN();
        Vec4f tangent_normal = (mat->normal_tex->Sample2D(v2f.texcoord) * 2.0f - 1.0f);
        normal = TBN * Vec3f(tangent_normal.x, tangent_normal.y, tangent_normal.z);
    } else {
        normal = v2f.normal.Normalize();
    }

    float metallic = mat->metallic;
    float roughness = mat->roughness;
    if (mat->metalroughness_tex) {
        Vec4f mr = mat->metalroughness_tex->Sample2D(v2f.texcoord);
        metallic = mr.b;
        roughness = mr.g;
    }

    float ao = 1.0f;
    if (mat->ao_tex) {
        ao = mat->ao_tex->Sample2D(v2f.texcoord).r;
    }

    Vec3f f0 = Vec3f::Lerp(mat->f0, albedo, metallic);

    Vec3f color(0.0f);
    if (mat->emission_tex) {
        color += mat->emission_tex->SampleRGB(v2f.texcoord);
    }

    Vec3f view_dir = (uniform_->camera_pos - v2f.world_position).Normalize();
    
    for (auto& light : uniform_->lights) {
        color += CalcLight(light, v2f, view_dir, normal, albedo, f0, roughness, metallic);
    }
    
    // Vec3f ambient = mat->ambient_color * albedo * ao;
    // color += ambient;

    Vec3f ambient = EvaluateIBL(view_dir, normal, f0, albedo, metallic, roughness, ao);
    color += ambient;
    
    // HDR tonemapping
    // https://knarkowicz.wordpress.com/2016/01/06/aces-filmic-tone-mapping-curve/
    color = ACESToneMapping(color, 1.0f);
    
    return LinearToGammaSpace(color);
}

}
