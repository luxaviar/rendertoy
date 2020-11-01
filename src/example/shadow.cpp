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
#include "common/color.h"

using namespace rendertoy;

void add_skybox(Pipeline& pipeline) {
    SkyboxMaterial* mat = pipeline.CreateMaterial<SkyboxMaterial>();
    Texture3D* skybox_tex = pipeline.CreateTexture3D("../assets/skybox/city_skybox.hdr", true);
    mat->skybox_tex = skybox_tex;

    Model model;
    Mesh mesh = Mesh::CreateBox(Vec3f::zero, 1.0f);
    mesh.material(mat);

    model.AddMesh(std::move(mesh));

    pipeline.SetSkybox(std::move(model));
}

void test_shadow(Pipeline& pipeline) {
    add_skybox(pipeline);

    PbrMaterial* mat = pipeline.CreateMaterial<PbrMaterial>();

    mat->f0 = { 0.04f };
    mat->ambient_color = { 0.09f };

    Texture2D* albedo_tex = pipeline.CreateTexture2D("../assets/helmet/helmet_albedo.png", true);
    mat->albedo_tex = albedo_tex;

    Texture2D* normal_tex = pipeline.CreateTexture2D("../assets/helmet/helmet_normal.png");
    mat->normal_tex = normal_tex;

    Texture2D* metalroughness_tex = pipeline.CreateTexture2D("../assets/helmet/helmet_metalroughness.png");
    mat->metalroughness_tex = metalroughness_tex;

    Texture2D* occlusion_tex = pipeline.CreateTexture2D("../assets/helmet/helmet_occlusion.png");
    mat->ao_tex = occlusion_tex;

    Texture2D* emission_tex = pipeline.CreateTexture2D("../assets/helmet/helmet_emission.png", true);
    mat->emission_tex = emission_tex;

    Texture3D* irradiance_tex = pipeline.CreateTexture3D("../assets/skybox/city_irradiance.hdr");
    mat->irradiance_tex = irradiance_tex;

    Texture3D* radiance_tex = pipeline.CreateTexture3D("../assets/skybox/city_radiance.hdr");
    mat->radiance_tex = radiance_tex;

    Texture2D* lut_tex = pipeline.CreateTexture2D("../assets/ibl_brdf_lut.png");
    mat->brdf_lut = lut_tex;

    Model model("../assets/helmet/helmet.obj");
    model.SetTRS(Vec3f(0.0f, 0.1f, 0.0f), Quaternion::AngleAxis(15, Vec3f::up), Vec3f(1.0f));
    auto& meshes = model.meshes();
    assert(meshes.size() > 0);
    meshes[0].material(mat);

    pipeline.AddModel(std::move(model));


    BlinnPhongMaterial* floor_mat = pipeline.CreateMaterial<BlinnPhongMaterial>();

    floor_mat->ka = { 0.005f, 0.005f, 0.005f };
    //mat->ks = { 0.7937f, 0.7937f, 0.7937f };
    floor_mat->ambient_color = { 0.04f, 0.04f, 0.04f };
    floor_mat->gloss = 150.0f;

    Texture2D* floor_main_tex = pipeline.CreateTexture2D("../assets/box/Wooden_box_01_BaseColor.png", true);
    floor_mat->main_tex = floor_main_tex;

    Model floor_model("../assets/box/Wooden_stuff.obj");
    //floor_model.SetTRS(Vec3f(0.0f, -2.0f, 1.0f), Quaternion::AngleAxis(90, Vec3f::right), Vec3f(0.2f));
    floor_model.SetTRS(Vec3f(0.0f, -3.0f, -0.0f), Quaternion::AngleAxis(0, Vec3f::right), Vec3f(4.0f));
    auto& floor_meshes = floor_model.meshes();
    assert(floor_meshes.size() > 0);
    floor_meshes[0].material(floor_mat);

    pipeline.AddModel(std::move(floor_model));

    Light light1;
    light1.color = { 1, 1, 1 };
    light1.intensity = 5.0f;
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
    light3.direction = Quaternion::AngleAxis(45, Vec3f::right) * Quaternion::AngleAxis(-45, Vec3f::up) * Vec3f::right;
    light3.type = LightType::kDirection;

    pipeline.AddLight(light1);
    //pipeline.AddLight(light2);
    pipeline.AddLight(light3);
}

int main(int argc, const char** argv) {
    set_flip_vertically_on_load(1);
    
    Pipeline pipeline;
    RenderTexture render_texture(1280, 720);
    render_texture.msaa(MSAALevel::k4x);
    render_texture.Clear(Buffers::kColor | Buffers::kDepth);
    pipeline.SetRenderTarget(&render_texture);

    RenderTexture shadow_texture(512, 512);
    shadow_texture.Clear(Buffers::kColor | Buffers::kDepth);
    pipeline.SetShadowTexture(&shadow_texture);
    pipeline.SetShadow(true);
    
    test_shadow(pipeline);
    
    Camera camera(60, 0.1, 50, { 0, 0, -3 }, Vec3f::zero, Vec3f::up);
    pipeline.Render(camera, Primitive::kTriangle);
    //pipeline.Render(camera, Primitive::kLine);

    Buffer<Col3U8> shadow_buffer(shadow_texture.width(), shadow_texture.height());
    shadow_texture.DepthToImage(shadow_buffer);

    Buffer<Col3U8> color_buffer(render_texture.width(), render_texture.height());
    render_texture.ColorToImage(color_buffer);

    write_png_image("output.png", color_buffer.width(), color_buffer.height(), 3, (const void*)color_buffer.data().data(), 0);
    write_png_image("output_shadow.png", shadow_buffer.width(), shadow_buffer.height(), 3, (const void*)shadow_buffer.data().data(), 0);
    return 0;
}
