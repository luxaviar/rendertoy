#pragma once

#include <vector>
#include "uncopyable.h"
#include "math/mat4.h"
#include "math/quat.h"
#include "mesh.h"

namespace  rendertoy {

class Model : private Uncopyable {
public:
    explicit Model(const char* filename);
    Model() : model_transform_(Matrix4x4::identity) {}
    Model(Model&& other) noexcept;

    void Swap(Model&& other) noexcept;

    void AddMesh(Mesh&& mesh);
    void SetTRS(const Vec3f &pos, const Quaternion& rotation, const Vec3f scale);

    const std::vector<Mesh>& meshes() const { return meshes_; }
    const Matrix4x4& model_transform() const { return model_transform_; }
    
private:
    std::vector<Mesh> meshes_;
    Matrix4x4 model_transform_;
};

} // namespace  rendertoy

