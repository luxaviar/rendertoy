#include "mesh.h"
#include "3rdparty/obj_loader.h"

namespace rendertoy {

Mesh Mesh::CreateBox(Vec3f center, float width) { 
    Vec3f vertices[] = {
        Vec3f(-0.5f, 0.5f, 0.5f), //0
        Vec3f(0.5f, 0.5f, 0.5f), //1
        Vec3f(-0.5f, -0.5f, 0.5f), //2
        Vec3f(0.5f, -0.5f, 0.5f), //3

        Vec3f(-0.5f, 0.5f, -0.5f), //4
        Vec3f(0.5f, 0.5f, -0.5f), //5
        Vec3f(-0.5f, -0.5f, -0.5f), //6
        Vec3f(0.5f, -0.5f, -0.5f), //7

        Vec3f(-0.5f, 0.5f, 0.5f),
        Vec3f(0.5f, 0.5f, 0.5f),
        Vec3f(0.5f, 0.5f, -0.5f),
        Vec3f(-0.5f, 0.5f, -0.5f),

        Vec3f(0.5f, -0.5f, 0.5f),
        Vec3f(-0.5f, -0.5f, 0.5f),
        Vec3f(-0.5f, -0.5f, -0.5f),
        Vec3f(0.5f, -0.5f, -0.5f),

        Vec3f(0.5f, 0.5f, 0.5f),
        Vec3f(0.5f, -0.5f, 0.5f),
        Vec3f(0.5f, 0.5f, -0.5f),
        Vec3f(0.5f, -0.5f, -0.5f),

        Vec3f(-0.5f, 0.5f, 0.5f), //0
        Vec3f(-0.5f, -0.5f, 0.5f), //2
        Vec3f(-0.5f, 0.5f, -0.5f), //4
        Vec3f(-0.5f, -0.5f, -0.5f), //6
    };

    for (auto& v : vertices) {
        v *= width;
        v += center;
    }

    int triangles[] = {
        0, 2, 1, //face front
        1, 2, 3,
        4, 5, 6, //face back
        5, 7, 6,
        8, 9, 11, //face top
        9, 10, 11,
        12, 13, 14, //face bottom
        12, 14, 15,
        16, 17, 18, //face right
        18, 17, 19,
        20, 22, 21, //face left
        21, 22, 23
    };
    
    Mesh mesh;
    for (auto v : vertices) {
        mesh.AddVertex(v);
    }
    for (int i = 0; i < 12; ++i) {
        mesh.AddTriangle(triangles[i * 3], triangles[i * 3 + 1], triangles[i * 3 + 2]); //we want back face for skybox
    }
    
    return mesh;
}

Mesh::Mesh(const char* filename) : material_(nullptr) {
    // Load .obj File
    objl::Loader loader;
    bool loadout = loader.LoadFile(filename);
    
    for(auto& mesh : loader.LoadedMeshes) {
        for (auto& vertex : mesh.Vertices) {
            AddVertex(Vec4f(vertex.Position.X, vertex.Position.Y, -vertex.Position.Z, 1.0f), 
                Vec4f(0, 0, 0, 255.0f),
                Vec3f(vertex.Normal.X, vertex.Normal.Y, -vertex.Normal.Z), 
                Vec2f(vertex.TextureCoordinate.X,
                    vertex.TextureCoordinate.Y < 0.0f ? 
                    -vertex.TextureCoordinate.Y : vertex.TextureCoordinate.Y)
            );
        }

        for (int i = 0; i < mesh.Indices.size(); i+=3) {
            auto i0 = mesh.Indices[i+2];
            auto i1 = mesh.Indices[i+1];
            auto i2 = mesh.Indices[i];

            AddTriangle(i0, i1, i2);

            auto& v0 = vertices_[i0];
            auto& v1 = vertices_[i1];
            auto& v2 = vertices_[i2];

            Vec3f tangent, bitangent;
            Vertex::CalcTangent(v0, v1, v2, tangent, bitangent);
            v0.tangent += tangent;
            v1.tangent += tangent;
            v2.tangent += tangent;

            v0.bitangent += bitangent;
            v1.bitangent += bitangent;
            v2.bitangent += bitangent;
        }

        for (int i = 0; i < vertices_.size(); ++i) {
            auto& v = vertices_[i];
            Vec3f normal = v.normal;
            Vec3f tangent = v.tangent;
            Vec3f bitangent = v.bitangent;

            tangent = (tangent - (normal.Dot(tangent) * normal)).Normalize();
            float c = normal.Cross(tangent).Dot(bitangent);
            if (c < 0.0f) {
                tangent *= -1.0f;
            }

            v.tangent = tangent;
        }
    }
}

Mesh::Mesh() : material_(nullptr) {

}

Mesh::Mesh(Mesh&& other) noexcept {
    vertices_.swap(other.vertices_);
    triangles_.swap(other.triangles_);
    std::swap(material_, other.material_);
}

void Mesh::AddVertex(Vec4f pos, Vec4f color, Vec3f normal, Vec2f uv) {
    vertices_.emplace_back(pos, color, normal, uv);
}

void Mesh::AddVertex(Vec3f pos, Vec3f color) {
    vertices_.emplace_back(pos, Vec4f(color.r, color.g, color.b, 1.0f), 
        Vec3f::up, Vec2f::zero);
}

void Mesh::AddTriangle(uint32_t v0, uint32_t v1, uint32_t v2) {
    triangles_.push_back({ v0, v1, v2 });
}

}
