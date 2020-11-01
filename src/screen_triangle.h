#pragma once

#include "vertex.h"
#include <cmath>

namespace rendertoy {

struct ScreenTriangle : private Uncopyable {
    ScreenTriangle(const VertexOut& v0_, const VertexOut& v1_, const VertexOut& v2_, RenderTexture* render_texture) : 
        v0(v0_), v1(v1_), v2(v2_),
        render_texture_(render_texture)
    {
        const Vec4f& p0 = v0.position;
        const Vec4f& p1 = v1.position;
        const Vec4f& p2 = v2.position;

        float min_x = math::Min(p0.x, math::Min(p1.x, p2.x));
        float min_y = math::Min(p0.y, math::Min(p1.y, p2.y));
        float max_x = math::Max(p0.x, math::Max(p1.x, p2.x));
        float max_y = math::Max(p0.y, math::Max(p1.y, p2.y));
        
        min.x = math::Max((int)std::floor(min_x), 0);
        min.y = math::Max((int)std::floor(min_y), 0);

        max.x = math::Min((int)std::ceil(max_x), render_texture_->width() - 1);
        max.y = math::Min((int)std::ceil(max_y), render_texture_->height() - 1);

        area2_reciprocal = 1.0f / math::Abs((p0.x - p1.x) * (p2.y - p1.y) - (p0.y - p1.y) * (p2.x - p1.x));

        edge[0] = {p1.x - p0.x, p1.y - p0.y, p0.x, p0.y}; //p0-p1
        edge[1] = {p2.x - p1.x, p2.y - p1.y, p1.x, p1.y}; //p1-p2
        edge[2] = {p0.x - p2.x, p0.y - p2.y, p2.x, p2.y}; //p2-p0
    }

    float TopLeftEdge(const Vec4f& e) const {
        return e.y > 0 || (e.y == 0 && e.x > 0) ? 0.0f : -1.0f;
    }

    float EdgeEquation(float x, float y, int idx, bool check_topleft=true) const {
        const Vec4f& e = edge[idx];
        float v = e.y* (x - e.z) - e.x * (y - e.w);
        if (v == 0.0f && check_topleft) {
            return TopLeftEdge(e);
        } else {
            return v;
        }
    }

    bool Inside(const Vec2f& pos, Vec3f& weight) const {
        float x = pos.x;
        float y = pos.y;

        float e0 = EdgeEquation(x, y, 1);
        if (e0 < 0.0f) return false;

        float e1 = EdgeEquation(x, y, 2);
        if (e1 < 0.0f) return false;

        float e2 = EdgeEquation(x, y, 0);
        if (e2 < 0.0f) return false;

        float alpha = e0 * area2_reciprocal;
        float beta = e1 * area2_reciprocal;
        float gamma = e2 * area2_reciprocal;

        //depth in view space reciprocal (z)
        float w_reciprocal = 1.0f / (alpha * v0.w_reciprocal + beta * v1.w_reciprocal + gamma * v2.w_reciprocal);
        weight[0] = alpha * v0.w_reciprocal * w_reciprocal;
        weight[1] = beta * v1.w_reciprocal * w_reciprocal;
        weight[2] = gamma * v2.w_reciprocal * w_reciprocal;

        return true;
    }

    bool Coverage(const Vec2f& pos, const Vec2i& pixel, int sub_sample, float& depth) const {
        Vec3f w;
        if (!Inside(pos, w)) {
            return false;
        }

        //depth in NDC [-1,1]
        float z_interpolated = v0.position.z * w[0] + v1.position.z * w[1] + v2.position.z * w[2];
        if (z_interpolated >= render_texture_->GetDepth(pixel.x, pixel.y, sub_sample)) {
            return false;
        }

        depth = z_interpolated;

        return true;
    }

    VertexOut Lerp(float w0, float w1, float w2) const {
        VertexOut o;
        o.w_reciprocal = v0.w_reciprocal * w0 + v1.w_reciprocal * w1 + v2.w_reciprocal * w2;
        o.position = v0.position * w0 + v1.position * w1 + v2.position * w2;
        o.world_position = v0.world_position * w0 + v1.world_position * w1 + v2.world_position * w2;
        o.color = v0.color * w0 + v1.color * w1 + v2.color * w2;
        o.normal = v0.normal * w0 + v1.normal * w1 + v2.normal * w2;
        o.tangent = v0.tangent * w0 + v1.tangent * w1 + v2.tangent * w2;
        o.texcoord = v0.texcoord * w0 + v1.texcoord * w1 + v2.texcoord * w2;
        o.shadow_coord = v0.shadow_coord * w0 + v1.shadow_coord * w1 + v2.shadow_coord * w2;

        return o;
    }

    VertexOut Rasterize(float x, float y) const {
        float e0 = EdgeEquation(x, y, 1, false);
        float e1 = EdgeEquation(x, y, 2, false);
        float e2 = EdgeEquation(x, y, 0, false);

        float alpha = e0 * area2_reciprocal;
        float beta = e1 * area2_reciprocal;
        float gamma = e2 * area2_reciprocal;

        //depth in view space reciprocal (z)
        float w_reciprocal = 1.0f / (alpha * v0.w_reciprocal + beta * v1.w_reciprocal + gamma * v2.w_reciprocal);
        float w0 = alpha * v0.w_reciprocal * w_reciprocal;
        float w1 = beta * v1.w_reciprocal * w_reciprocal;
        float w2 = gamma * v2.w_reciprocal * w_reciprocal;

        return Lerp(w0, w1, w2);
    }

    float area2_reciprocal;
    Vec4f edge[3];
    Vec2i min;
    Vec2i max;
    const VertexOut& v0;
    const VertexOut& v1;
    const VertexOut& v2;
    RenderTexture* render_texture_;
};

}
