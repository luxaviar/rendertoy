#include "model.h"
#include "3rdparty/obj_loader.h"

namespace rendertoy {
Model::Model(const char* filename) : Model() {
    // Load .obj File
    // Mesh mesh(filename);
    name_ = filename;
    meshes_.emplace_back(filename);
}

Model::Model(Model&& other) noexcept {
    meshes_.swap(other.meshes_);
    name_.swap(other.name_);
    model_transform_ = other.model_transform_;
}

void Model::Swap(Model&& other) noexcept {
    meshes_.swap(other.meshes_);
    name_.swap(other.name_);
    auto tmp = model_transform_;
    model_transform_ = other.model_transform_;
    other.model_transform_ = tmp;
}

void Model::AddMesh(Mesh&& mesh) {
    meshes_.emplace_back(std::move(mesh));
}

void Model::SetTRS(const Vec3f &pos, const Quaternion& rotation, const Vec3f scale) {
    model_transform_ = Matrix4x4::TRS(pos, rotation, scale);
}

}