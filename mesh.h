#pragma once

#include <vector>
#include "uncopyable.h"
#include "triangle.h"
#include "math/mat4.h"
#include "math/quat.h"
#include "vertex.h"
#include "material/material.h"

namespace  rendertoy {

class Mesh : private Uncopyable {
public:
    using TriangleIndex = std::array<uint32_t, 3>;

    static Mesh CreateBox(Vec3f center, float width);

    explicit Mesh(const char* filename);
    Mesh();
    Mesh(Mesh&& other) noexcept;
    
    void material(Material* mat) const { material_ = mat; }
    const Material* material() const { return material_; }

    void AddVertex(Vec3f pos, Vec3f color=Vec3f::zero);
    void AddVertex(Vec4f pos, Vec4f color, Vec3f normal, Vec2f uv);
    void AddTriangle(uint32_t idx1, uint32_t idx2, uint32_t idx3);

    const std::vector<Vertex>& vertices() const { return vertices_; }
    const std::vector<TriangleIndex>& triangles() const { return triangles_; }
    
private:
    std::vector<Vertex> vertices_;
    std::vector<TriangleIndex> triangles_;
    mutable Material* material_;
};

} // namespace  rendertoy

