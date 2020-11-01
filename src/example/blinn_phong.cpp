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

void test_simple_mesh(Pipeline& pipeline) {
    VertLitMaterial* mat = pipeline.CreateMaterial<VertLitMaterial>();
    Mesh mesh;
    mesh.material(mat);

    Vec3f color1 = GammaToLinearSpace(Vec3f(217.0 / 255.0, 238.0 / 255.0, 185.0 / 255.0));
    Vec3f color2 = GammaToLinearSpace(Vec3f(185.0 / 255.0, 217.0 / 255.0, 238.0 / 255.0));

    mesh.AddVertex(Vec3f(-2, 0, 3), color1);
    mesh.AddVertex(Vec3f(0, 2, 3), color1);
    mesh.AddVertex(Vec3f(2, 0, 3), color1);

    mesh.AddVertex(Vec3f(-2, 0, 3), color1);
    mesh.AddVertex(Vec3f(2, 0, 3), color1);
    mesh.AddVertex(Vec3f(0, -2, 3), color1);

    mesh.AddVertex(Vec3f(-1, 0.5, 4), color2);
    mesh.AddVertex(Vec3f(2.5, 1.5, 4), color2);
    mesh.AddVertex(Vec3f(4.5, -1, 4), color2);

    mesh.AddVertex(Vec3f(-4.5, -1, 4), color2);
    mesh.AddVertex(Vec3f(-2.5, 1.5, 4), color2);
    mesh.AddVertex(Vec3f(1, 0.5, 4), color2);

    mesh.AddVertex(Vec3f(-4.5, -1, 4), color2);
    mesh.AddVertex(Vec3f(1, 0.5, 4), color2);
    mesh.AddVertex(Vec3f(3, -1, 4), color2);

    mesh.AddVertex(Vec3f(3, -1, 4), color2);
    mesh.AddVertex(Vec3f(1, 0.5, 4), color2);
    mesh.AddVertex(Vec3f(3, 1, 4), color2);

    //mesh.AddVertex(Vec3f(-1, 0.5, 4), color2);
    //mesh.AddVertex(Vec3f(3.5, 1.6, 4), color2);
    //mesh.AddVertex(Vec3f(1.5, 0.3, 4), color2);
    
        
    mesh.AddTriangle(0, 1, 2);
    //mesh.AddTriangle(3, 4, 5);
    mesh.AddTriangle(6, 7, 8);
    //mesh.AddTriangle(9, 10, 11);
    //mesh.AddTriangle(12, 13, 14);
    //mesh.AddTriangle(15, 16, 17);
    //mesh.AddTriangle(18, 19, 20);

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

    Texture2D* main_tex = pipeline.CreateTexture2D("../assets/helmet/helmet_albedo.png", true);
    mat->main_tex = main_tex;

    Texture2D* normal_tex = pipeline.CreateTexture2D("../assets/helmet/helmet_normal.png");
    mat->normal_tex = normal_tex;

    Model model("../assets/helmet/helmet.obj");
    model.SetTRS(Vec3f(0.0f, 0.1f, 0.0f), Quaternion::AngleAxis(25, Vec3f::up), Vec3f(1.0f));
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

int main(int argc, const char** argv) {
    set_flip_vertically_on_load(1);
    
    Pipeline pipeline;
    RenderTexture render_texture(1280, 720);
    render_texture.msaa(MSAALevel::k4x);
    render_texture.Clear(Buffers::kColor | Buffers::kDepth);
    pipeline.SetRenderTarget(&render_texture);
    
    //test_simple_mesh(pipeline);
    test_blinnphong(pipeline);
    
    Camera camera(40, 0.1, 50, { 0, 0, -3 }, Vec3f::zero, Vec3f::up);
    pipeline.Render(camera, Primitive::kTriangle);
    //pipeline.Render(camera, Primitive::kLine);

    Buffer<Col3U8> color_buffer(render_texture.width(), render_texture.height());
    render_texture.ColorToImage(color_buffer);

    write_png_image("output.png", color_buffer.width(), color_buffer.height(), 3, (const void*)color_buffer.data().data(), 0);
    return 0;
}
