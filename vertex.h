#pragma once

#include <vector>
#include "uncopyable.h"
#include "math/vec2.h"
#include "math/vec3.h"
#include "math/vec4.h"
#include "math/util.h"
#include "math/axis.h"
#include "math/mat3.h"

namespace  rendertoy
{

struct Vertex {
    Vertex() : tangent(0.0f), bitangent(0.0f) {}
    Vertex(Vec3f pos, Vec4f col, Vec3f norm, Vec2f uv) :
        position(pos.x, pos.y, pos.z, 1.0f), 
        color(col), normal(norm), tangent(0.0f), bitangent(0.0f), texcoord(uv)
    {
    }
    Vertex(Vec4f pos, Vec4f col, Vec3f norm, Vec2f uv) : 
        position(pos), color(col), normal(norm), tangent(0.0f), bitangent(0.0f), texcoord(uv) {}

    Vec4f position;
    Vec4f color;
    Vec3f normal;
    Vec3f tangent;
    Vec3f bitangent;
    Vec2f texcoord;

    static void CalcTangent(const Vertex & v0, const Vertex & v1, const Vertex & v2, Vec3f& tangent, Vec3f& bitangent) {
        Vec4f q1 = v1.position - v0.position;
        Vec4f q2 = v2.position - v0.position;

        Vec2f duv1 = v1.texcoord - v0.texcoord;
        Vec2f duv2 = v2.texcoord - v0.texcoord;
        float du1 = duv1.u;
        float dv1 = duv1.v;
        float du2 = duv2.u;
        float dv2 = duv2.v;
        float r = 1.0f / (du1 * dv2 - du2 * dv1);

        tangent.x = r * (dv2 * q1.x - dv1 * q2.x);
        tangent.y = r * (dv2 * q1.y - dv1 * q2.y);
        tangent.z = r * (dv2 * q1.z - dv1 * q2.z);

        bitangent.x = r * (du1 * q2.x - du2 * q1.x);
        bitangent.y = r * (du1 * q2.y - du2 * q1.y);
        bitangent.z = r * (du1 * q2.z - du2 * q1.z);

        //tangent.Normalized();
        //bitangent.Normalize();
    }
};

struct VertexOut {
    float w_reciprocal;
    Vec4f position; //SV_POSTION
    Vec3f world_position;
    Vec4f color;
    mutable Vec3f normal;
    mutable Vec3f tangent;
    Vec2f texcoord;
    
    bool InsideFrustum(float near, float far) const {
        if (position.w < near || position.w > far) return false;

        float w = math::Abs(position.w);
        return math::Abs(position.x) <= w && math::Abs(position.y) <= w && math::Abs(position.z) <= w;
    }

    Matrix3x3 TBN() const {
        normal.Normalized();
        tangent.Normalized();
        Vec3f binormal = normal.Cross(tangent);

        return Matrix3x3 {
            tangent.x, binormal.x, normal.x,
            tangent.y, binormal.y, normal.y,
            tangent.z, binormal.z, normal.z,
        };
    }
};

struct ClipPlane {
    math::Axis axis;
    float sign;

    bool Inside(const Vec4f& pos) const {
        float c = pos[static_cast<int>(axis)];
        return sign > 0 ? c <= pos.w : c >= -pos.w;
    }

    float Intersect(const Vec4f& p0, const Vec4f& p1) const {
        float c0 = p0[static_cast<int>(axis)];
        float c1 = p1[static_cast<int>(axis)];
        float t = (c0 - sign * p0.w) / (sign * (p1.w - p0.w) - (c1 - c0));
        return t;
    }
};

}
