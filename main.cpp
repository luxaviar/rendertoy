#include <iostream>
#include <vector>
#include "image.h"
#include "math/util.h"
#include "math/vec3.h"
#include "math/mat4.h"
#include "math/mat3.h"
#include "model.h"
#include "camera.h"
#include "pipeline.h"
#include "texture2D.h"
#include "texture3D.h"
#include "material/vertlit_material.h"
#include "material/blinnphong_material.h"
#include "material/normal_material.h"
#include "material/pbr_material.h"
#include "material/skybox_material.h"

using namespace rendertoy;

void test_simple_mesh(Pipeline& pipeline) {
    VertLitMaterial* mat = pipeline.CreateMaterial<VertLitMaterial>();
    Mesh mesh;
    mesh.material(mat);

    mesh.AddVertex(Vec3f(-2, 0, 1), Vec3f(217.0, 238.0, 185.0));
    mesh.AddVertex(Vec3f(0, 2, 1), Vec3f(217.0, 238.0, 185.0));
    mesh.AddVertex(Vec3f(2, 0, 1), Vec3f(217.0, 238.0, 185.0));
    mesh.AddVertex(Vec3f(-1, 0.5, 2), Vec3f(185.0, 217.0, 238.0));
    mesh.AddVertex(Vec3f(2.5, 1.5, 2), Vec3f(185.0, 217.0, 238.0));
    mesh.AddVertex(Vec3f(4.5, -1, 2), Vec3f(185.0, 217.0, 238.0));
    mesh.AddTriangle(1, 0, 2);
    mesh.AddTriangle(4, 3, 5);

    Model model;
    model.AddMesh(std::move(mesh));

    pipeline.AddModel(std::move(model));
}

void test_blinnphong(Pipeline& pipeline) {
    BlinnPhongMaterial* mat = pipeline.CreateMaterial<BlinnPhongMaterial>();

    mat->ka = { 0.005f, 0.005f, 0.005f };
    //mat->ks = { 0.7937f, 0.7937f, 0.7937f };
    mat->ambient_color = { 0.04f, 0.04f, 0.04f };
    mat->gloss = 150.0f;

    Texture2D* main_tex = pipeline.CreateTexture2D("../assets/helmet/helmet_albedo.tga", true);
    mat->main_tex = main_tex;

    Texture2D* normal_tex = pipeline.CreateTexture2D("../assets/helmet/helmet_normal.tga");
    mat->normal_tex = normal_tex;

    Model model("../assets/helmet/helmet.obj");
    model.SetTRS(Vec3f::zero, Quaternion::AngleAxis(30, Vec3f::up), Vec3f(1.0f));
    auto& meshes = model.meshes();
    assert(meshes.size() > 0);
    meshes[0].material(mat);

    pipeline.AddModel(std::move(model));

    Light light1;
    light1.color = { 1, 1, 1 };
    light1.intensity = 50.0f;
    light1.position = { 5, 5, 0 };
    light1.type = LightType::kPoint;

    Light light2;
    light2.color = { 1, 1, 1 };
    light2.intensity = 25.0f;
    light2.position = { -5, 5, 0 };
    light2.type = LightType::kPoint;

    Light light3;
    light3.color = { 1, 1, 1 };
    light3.intensity = 1.0f;
    light3.direction = Quaternion::AngleAxis(-45, Vec3f::up) * Vec3f::right;
    light3.type = LightType::kDirection;

    pipeline.AddLight(light1);
    //pipeline.AddLight(light2);
    pipeline.AddLight(light3);
}

void add_skybox(Pipeline& pipeline) {
    SkyboxMaterial* mat = pipeline.CreateMaterial<SkyboxMaterial>();
    Texture3D* skybox_tex = pipeline.CreateTexture3D("../assets/skybox/valley_skybox.hdr");
    mat->skybox_tex = skybox_tex;

    Model model;
    Mesh mesh = Mesh::CreateBox(Vec3f::zero, 1.0f);
    mesh.material(mat);

    model.AddMesh(std::move(mesh));

    pipeline.SetSkybox(std::move(model));
}

void test_pbr(Pipeline& pipeline) {
    add_skybox(pipeline);

    PbrMaterial* mat = pipeline.CreateMaterial<PbrMaterial>();

    mat->f0 = { 0.04f };
    mat->ambient_color = {0.09f};
    
    Texture2D* albedo_tex = pipeline.CreateTexture2D("../assets/helmet/helmet_albedo.png", true);
    mat->albedo_tex = albedo_tex;

    Texture2D* normal_tex = pipeline.CreateTexture2D("../assets/helmet/helmet_normal.png");
    mat->normal_tex = normal_tex;

    Texture2D* metalroughness_tex = pipeline.CreateTexture2D("../assets/helmet/helmet_metalroughness.png", true);
    mat->metalroughness_tex = metalroughness_tex;

    Texture2D* occlusion_tex = pipeline.CreateTexture2D("../assets/helmet/helmet_occlusion.png");
    mat->ao_tex = occlusion_tex;

    Texture2D* emission_tex = pipeline.CreateTexture2D("../assets/helmet/helmet_emission.png", true);
    mat->emission_tex = emission_tex;

    Texture3D* irradiance_tex = pipeline.CreateTexture3D("../assets/skybox/valley_irradiance.hdr");
    mat->irradiance_tex = irradiance_tex;

    Texture3D* radiance_tex = pipeline.CreateTexture3D("../assets/skybox/valley_radiance.hdr");
    mat->radiance_tex = radiance_tex;

    Texture2D* lut_tex = pipeline.CreateTexture2D("../assets/ibl_brdf_lut.png");
    mat->brdf_lut = lut_tex;

    Model model("../assets/helmet/helmet.obj");
    model.SetTRS(Vec3f::zero, Quaternion::AngleAxis(-105, Vec3f::up), Vec3f(1.0f));
    auto& meshes = model.meshes();
    assert(meshes.size() > 0);
    meshes[0].material(mat);

    pipeline.AddModel(std::move(model));
    
    Light light1;
    light1.color = { 1, 1, 1 };
    light1.intensity = 50.0f;
    light1.position = { -2, 0, 3 };
    light1.type = LightType::kPoint;

    Light light2;
    light2.color = { 1, 1, 1 };
    light2.intensity = 3.0f;
    //light2.direction = Quaternion::AngleAxis(45, Vec3f::right) * Vec3f::back;
    light2.direction = Vec3f::back;
    light2.type = LightType::kDirection;

    //pipeline.AddLight(light1);
    pipeline.AddLight(light2);
}

int main(int argc, const char** argv) {
    set_flip_vertically_on_load(1);
    
    Pipeline pipeline;
    RenderTexture render_texture(1280, 720);
    render_texture.Clear(Buffers::kColor | Buffers::kDepth);
    pipeline.SetRenderTarget(&render_texture);
    
    //test_simple_mesh(pipeline);
    //test_blinnphong(pipeline);
    test_pbr(pipeline);
    
    Camera camera(27, 0.1, 50, { 3, 0, 3 }, Vec3f::zero, Vec3f::up);
    pipeline.Render(camera, Primitive::kTriangle);
    //pipeline.Render(camera, Primitive::kLine);

    Buffer<Col3U8> color_buffer(render_texture.width(), render_texture.height());
    render_texture.ConvertToImage(color_buffer);

    write_png_image("output.png", color_buffer.width(), color_buffer.height(), 3, (const void*)color_buffer.data().data(), 0);
    return 0;
}
