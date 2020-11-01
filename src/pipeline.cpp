#include "pipeline.h"
#include <algorithm>
#include <vector>
#include <math.h>
#include "math/vec4.h"
#include "math/vec2.h"
#include "material/vertlit_material.h"
#include "texture2D.h"
#include "shader/shadow_shader.h"

namespace rendertoy {

Pipeline::Pipeline() : cast_shadow_(false), render_texture_(nullptr), render_type_(Primitive::kTriangle) {
    default_material_ = new VertLitMaterial();
}

Pipeline::~Pipeline() {
    delete default_material_;
    for (auto t : texture2Ds_) {
        delete t;
    }

    for (auto m : materials_) {
        delete m;
    }
}

Texture2D* Pipeline::CreateTexture2D(const char* file, bool sRGB) {
    auto t = new Texture2D(file, sRGB);
    texture2Ds_.push_back(t);
    return t;
}

Texture3D* Pipeline::CreateTexture3D(const char* file, bool sRGB) {
    auto t = new Texture3D(file, sRGB);
    texture3Ds_.push_back(t);
    return t;
}

void Pipeline::AddModel(Model&& model) {
    models_.emplace_back(std::move(model));
}

void Pipeline::AddLight(const Light& light) {
    lights_.push_back(light);
}

void Pipeline::SetSkybox(Model&& skybox) {
    sky_box_.Swap(std::move(skybox));
}

void Pipeline::SetShadow(bool on) {
    cast_shadow_ = on;
}

void Pipeline::CastShadow(Uniform& u) {
    float width = 7.0f;
    float height = 7.0f;
    float near = -3.0f;
    float far = 3.0f;

    u.shadow_light_->SetupShadow(width, height, near, far, shadow_texture_);
    u.view = u.shadow_light_->view_matrix;
    u.projection = u.shadow_light_->project_matrix;
    u.vp = u.shadow_light_->vp_matrix;

    Graphics* graphic = Graphics::Instance();
    graphic->SetClipDistance(near, far);
    graphic->SetRenderTarget(shadow_texture_);
    graphic->SetRenderType(Primitive::kTriangle);

    for (auto& model : models_) {
        DrawModel(model, u, ShadowShader::Instance());
    }
}

void Pipeline::RenderScene(Uniform& u, Camera& camera, Primitive type) {
    render_type_ = type;
    
    int width = render_texture_->width();
    int height = render_texture_->height();
    camera.aspect = (float)width / height;

    u.view = camera.view_matrix();
    u.projection = camera.projection_matrix();
    u.vp = u.projection * u.view;
    u.camera_pos = camera.pos;
    Graphics* graphic = Graphics::Instance();

    graphic->SetClipDistance(camera.near, camera.far);
    graphic->SetRenderTarget(render_texture_);
    graphic->SetRenderType(type);

    for (auto& model : models_) {
        DrawModel(model, u);
    }

    DrawModel(sky_box_, u);
}

void Pipeline::Render(Camera& camera, Primitive type) {
    Uniform u;
    u.lights.insert(u.lights.begin(), lights_.begin(), lights_.end());
    u.shadow_light_ = nullptr;

    if (cast_shadow_ && shadow_texture_) {
        for (auto& light : u.lights) {
            if (light.type == LightType::kDirection) {
                u.shadow_light_ = &light;
                break;
            }
        }
    }

    Graphics* graphic = Graphics::Instance();

    if (u.shadow_light_) {
        CastShadow(u);
    }

    RenderScene(u, camera, type);

    DrawModel(sky_box_, u);
}

void Pipeline::DrawModel(const Model& model, Uniform& u, Shader* replace_shader) {
    Graphics* graphic = Graphics::Instance();
    u.model = model.model_transform();
    for (auto& mesh : model.meshes()) {
        auto& vertices = mesh.vertices();
        auto& triangles = mesh.triangles();

        u.mvp = u.vp * u.model;
        if (u.shadow_light_) {
            u.shadow_light_->mvp = u.shadow_light_->vp_matrix * u.model;
        }

        u.mat = mesh.material();
        if (!u.mat) {
            u.mat = default_material_;
        }

        if (replace_shader) {
            replace_shader->uniform(&u);
            graphic->SetShader(replace_shader);

            for (auto& tri : triangles) {
                graphic->DrawTriangle(
                    vertices[tri[0]],
                    vertices[tri[1]],
                    vertices[tri[2]]
                );
            }
        } else {
            for (Shader* shader : u.mat->pass()) {
                shader->uniform(&u);

                graphic->SetShader(shader);

                for (auto& tri : triangles) {
                    graphic->DrawTriangle(
                        vertices[tri[0]],
                        vertices[tri[1]],
                        vertices[tri[2]]
                    );
                }
            }
        }
    }
}

}
