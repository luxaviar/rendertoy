#include "pipeline.h"
#include <algorithm>
#include <vector>
#include <math.h>
#include "math/vec4.h"
#include "math/vec2.h"
#include "material/vertlit_material.h"
#include "texture2D.h"

namespace rendertoy {

Pipeline::Pipeline() : render_texture_(nullptr), render_type_(Primitive::kTriangle) {
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

Texture3D* Pipeline::CreateTexture3D(const char* file) {
    auto t = new Texture3D(file);
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

void Pipeline::Render(Camera& camera, Primitive type) {
    render_type_ = type;
    
    int width = render_texture_->width();
    int height = render_texture_->height();
    camera.aspect = (float)width / height;

    auto view = camera.view_matrix();
    auto projection = camera.projection_matrix();

    Uniform u;
    u.view = view;
    u.projection = projection;
    u.camera_pos = camera.pos;
    u.lights.insert(u.lights.begin(), lights_.begin(), lights_.end());

    Graphics* graphic = Graphics::Instance();
    graphic->SetCamera(&camera);
    graphic->SetRenderTarget(render_texture_);
    graphic->SetRenderType(type);

    for (auto& model : models_) {
        DrawModel(model, u);
    }

    DrawModel(sky_box_, u);
}

void Pipeline::DrawModel(const Model& model, Uniform& u) {
    Graphics* graphic = Graphics::Instance();
    u.model = model.model_transform();
    for (auto& mesh : model.meshes()) {
        auto& vertices = mesh.vertices();
        auto& triangles = mesh.triangles();

        u.mvp = u.projection * u.view * u.model;
        u.mat = mesh.material();
        if (!u.mat) {
            u.mat = default_material_;
        }

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
