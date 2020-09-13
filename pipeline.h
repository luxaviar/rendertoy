#pragma once

#include <vector>
#include <map>
#include "uncopyable.h"
#include "model.h"
#include "camera.h"
#include "rendertexture.h"
#include "graphics.h"
#include "light.h"
#include "material/material.h"
#include "texture3D.h"

namespace rendertoy {

class Pipeline : private Uncopyable {
public:
    Pipeline();
    ~Pipeline();

    template<typename T>
    T* CreateMaterial() {
        T* mat = new T();
        materials_.push_back(mat);
        return mat;
    }

    Texture2D* CreateTexture2D(const char* file, bool sRGB=false);
    Texture3D* CreateTexture3D(const char* file);

    void AddModel(Model&& model);
    void AddLight(const Light& light);
    void SetSkybox(Model&& skybox);

    void Render(Camera& camera, Primitive type);
    
    void SetRenderTarget(RenderTexture* render_texture) { render_texture_ = render_texture;  }
    RenderTexture* GetRenderTexture() { return render_texture_; }

private:
    void DrawModel(const Model& model, Uniform& u);

    std::vector<Model> models_;
    std::vector<Light> lights_;
    std::vector<Material*> materials_;
    std::vector<Texture2D*> texture2Ds_;
    std::vector<Texture3D*> texture3Ds_;

    Model sky_box_;
    Material* default_material_;
    RenderTexture* render_texture_;
    Primitive render_type_;
};

}
