
#include "triangle.h"
#include <algorithm>
#include <array>
#include "math/util.h"

namespace rendertoy {

Triangle::Triangle() {
    v[0] = {0,0,0};
    v[1] = {0,0,0};
    v[2] = {0,0,0};

    color[0] = {0.0, 0.0, 0.0};
    color[1] = {0.0, 0.0, 0.0};
    color[2] = {0.0, 0.0, 0.0};

    tex_coords[0] = {0.0, 0.0};
    tex_coords[1] = {0.0, 0.0};
    tex_coords[2] = {0.0, 0.0};
}

void Triangle::SetVertex(int idx, float x, float y, float z) {
    v[idx].x = x;
    v[idx].y = y;
    v[idx].z = z;
}

void Triangle::SetVertex(int idx, const Vec3f& ver){
    v[idx] = ver;
}

void Triangle::SetVertex(int idx, const Vec4f& ver){
    v[idx] = {ver.x, ver.y, ver.z};
}

void Triangle::SetNormal(int idx, const Vec3f& n){
    normal[idx] = n;
}

void Triangle::SetColor(int idx, float r, float g, float b) {
    if((r<0.0) || (r>255.) ||
       (g<0.0) || (g>255.) ||
       (b<0.0) || (b>255.)) {
        fprintf(stderr, "ERROR! Invalid color values");
        fflush(stderr);
        exit(-1);
    }

    color[idx] = Vec3f((float)r/255.0f, (float)g/255.0f, (float)b/255.0f);
    return;
}

void Triangle::SetColor(int idx, const Vec3f& col) {
    SetColor(idx, col.r, col.g, col.b);
}

void Triangle::SetColor(int idx, const Vec4f& col) {
    SetColor(idx, col.r, col.g, col.b);
}

void Triangle::SetTexCoord(int idx, float u, float v) {
    tex_coords[idx] = Vec2f(u, v);
}

std::array<Vec4f, 3> Triangle::ToVector4() const {
    std::array<Vec4f, 3> res;
    std::transform(std::begin(v), std::end(v), res.begin(), [](auto& vec) { return Vec4f(vec.x, vec.y, vec.z, 1.f); });
    return res;
}

float Triangle::Area2() const {
    return math::Abs((v[0].x - v[1].x) * (v[2].y - v[1].y) - (v[0].y - v[1].y) * (v[2].x - v[1].x));
}

}
