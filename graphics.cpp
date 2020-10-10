#include "graphics.h"
#include <algorithm>
#include <vector>
#include <cmath>
#include <cassert>
#include "shader/shader.h"
#include "camera.h"

namespace rendertoy {

Graphics::Graphics() : 
    near_(0.1f),
    far_(50.0f),
    write_depth_(true),
    cull_(CullMode::kBack),
    shader_(nullptr),
    render_texture_(nullptr),
    render_type_(Primitive::kLine) 
{

}

void Graphics::SetRenderTarget(RenderTexture* rt) {
    render_texture_ = rt;
}

void Graphics::SetShader(const Shader* shader) {
    shader_ = shader;
    SetWriteDepth(shader_->write_depth());
    SetCullMode(shader_->cull());
}

void Graphics::SetCamera(const Camera* camera) {
    near_ = camera->near;
    far_ = camera->far;
}

void Graphics::SetRenderType(Primitive type) {
    render_type_ = type;
}

void Graphics::SetWriteDepth(bool on) {
    write_depth_ = on;
}

void Graphics::SetCullMode(CullMode mode) { 
    cull_ = mode;
}

float Graphics::EdgeFunction(float x, float y, const Vec4f& v0, const Vec4f& v1) {
    return (v1.y - v0.y) * (x - v0.x) - (v1.x - v0.x) * (y - v0.y);
}

bool Graphics::InsideTriangle(const Vec2f& pos, const VertexOut& v0, const VertexOut& v1, const VertexOut& v2, 
    float area2_reciprocal, Vec3f& weight) {
    const Vec4f& p0 = v0.position;
    const Vec4f& p1 = v1.position;
    const Vec4f& p2 = v2.position;

    float x = pos.x;
    float y = pos.y;

    float e0 = EdgeFunction(x, y, p1, p2);
    if (e0 < 0.0f) return false;

    float e1 = EdgeFunction(x, y, p2, p0);
    if (e1 < 0.0f) return false;

    float e2 = EdgeFunction(x, y, p0, p1);
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

VertexOut Graphics::Lerp(const VertexOut& v0, const VertexOut& v1, float w) {
    VertexOut o;
    o.position = Vec4f::Lerp(v0.position, v1.position, w);
    o.world_position = Vec3f::Lerp(v0.world_position, v1.world_position, w);
    o.color = Vec4f::Lerp(v0.color, v1.color, w);
    o.normal = Vec3f::Lerp(v0.normal, v1.normal, w);
    o.tangent = Vec3f::Lerp(v0.tangent, v1.tangent, w);
    o.texcoord = Vec2f::Lerp(v0.texcoord, v1.texcoord, w);
    
    return o;
}

VertexOut Graphics::RasterizeLerp(const VertexOut& v0, const VertexOut& v1, const VertexOut& v2, float w0, float w1, float w2) {
    VertexOut o;
    o.position = v0.position * w0 + v1.position * w1 + v2.position * w2;
    o.world_position = v0.world_position * w0 + v1.world_position * w1 + v2.world_position * w2;
    o.color = v0.color * w0 + v1.color * w1 + v2.color * w2;
    o.normal = v0.normal * w0 + v1.normal * w1 + v2.normal * w2;
    o.tangent = v0.tangent * w0 + v1.tangent * w1 + v2.tangent * w2;
    o.texcoord = v0.texcoord * w0 + v1.texcoord * w1 + v2.texcoord * w2;
    
    return o;
}

bool Graphics::Cull(const VertexOut& v0, const VertexOut& v1, const VertexOut& v2) const {
    bool is_front = EdgeFunction(v2.position.x, v2.position.y, v0.position, v1.position) > 0.0f;
    return cull_ == CullMode::kFront ? is_front : !is_front;
}

void Graphics::Clip(const VertexOut& v0, const VertexOut& v1, const VertexOut& v2, std::vector<VertexOut>& output) {
    output.push_back(v0);
    output.push_back(v1);
    output.push_back(v2);

    if (v0.InsideFrustum(near_, far_) && v1.InsideFrustum(near_, far_) && v2.InsideFrustum(near_, far_)) {
        return;
    }

    for (auto& plane : kClipPlanes) {
        clip_input_.swap(output);
        output.clear();

        for (int j = 0; j < clip_input_.size(); j++) {
            const VertexOut& current = clip_input_[j];
            int last_idx = j - 1;
            if (last_idx < 0) {
                last_idx = clip_input_.size() - 1;
            }
            const VertexOut& last = clip_input_[last_idx];

            if (plane.Inside(current.position)) {
                if (!plane.Inside(last.position)) {
                    float t = plane.Intersect(current.position, last.position);
                    VertexOut intersect = Lerp(current, last, t);
                    output.push_back(intersect);
                }
                output.push_back(current);
            } else if (plane.Inside(last.position)) {
                float t = plane.Intersect(last.position, current.position);
                VertexOut intersect = Lerp(last, current, t);
                output.push_back(intersect);
            }
        }
    }

    clip_input_.clear();
}

void Graphics::DrawTriangle(const Vertex& v0, const Vertex& v1, const Vertex& v2) {
    assert(shader_);
    assert(render_texture_);

    //vert shader
    VertexOut vo0 = shader_->Vert(v0);
    VertexOut vo1 = shader_->Vert(v1);
    VertexOut vo2 = shader_->Vert(v2);

    //Cliping
    Clip(vo0, vo1, vo2, clip_output_);

    //Homogeneous division
    for (auto& vert : clip_output_) {
        vert.w_reciprocal = 1.0f / vert.position.w;
        vert.position *= vert.w_reciprocal;
    }

    //Viewport transformation
    int width = render_texture_->width();
    int height = render_texture_->height();
    for (auto& vert : clip_output_) {
        Vec4f& pos = vert.position;
        pos.x = 0.5f * width * (pos.x + 1.0f);
        pos.y = 0.5f * height * (pos.y + 1.0f);
        pos.z = 0.5f * (pos.z + 1.0f);
    }

    if (clip_output_.size() == 0) return;

    //rasterize
    for (int i = 0; i < clip_output_.size() - 3 + 1; ++i) {
        auto& o0 = clip_output_[0];
        auto& o1 = clip_output_[i + 1];
        auto& o2 = clip_output_[i + 2];
        
        bool is_front = EdgeFunction(o2.position.x, o2.position.y, o0.position, o1.position) > 0.0f;

        if ((cull_ == CullMode::kBack && !is_front) ||
            (cull_ == CullMode::kFront && is_front))
            continue;

        if (render_type_ == Primitive::kLine) {
            DrawLine(o0.position, o1.position, Vec4f(0.0f, 1.0f, 0.0f, 1.0f));
            DrawLine(o1.position, o2.position, Vec4f(0.0f, 1.0f, 0.0f, 1.0f));
            DrawLine(o2.position, o0.position, Vec4f(0.0f, 1.0f, 0.0f, 1.0f));
        } else {
            if (is_front) {
                RasterizeTriangle(o0, o1, o2);
            } else {
                RasterizeTriangle(o0, o2, o1);
            }
        }
    }

    clip_output_.clear();
}

void Graphics::RasterizeTriangle(const VertexOut& v0, const VertexOut& v1, const VertexOut& v2) {
    const Vec4f& p0 = v0.position;
    const Vec4f& p1 = v1.position;
    const Vec4f& p2 = v2.position;
    
    float min_x = math::Min(p0.x, math::Min(p1.x, p2.x));
    float min_y = math::Min(p0.y, math::Min(p1.y, p2.y));
    float max_x = math::Max(p0.x, math::Max(p1.x, p2.x));
    float max_y = math::Max(p0.y, math::Max(p1.y, p2.y));
    
    Vec2i bound_min;
    Vec2i bound_max;
    bound_min.x = math::Max((int)std::floor(min_x), 0);
    bound_min.y = math::Max((int)std::floor(min_y), 0);

    bound_max.x = math::Min((int)std::ceil(max_x), render_texture_->width() - 1);
    bound_max.y = math::Min((int)std::ceil(max_y), render_texture_->height() - 1);

    //FIXME: what about micro triangle?
    float area2_reciprocal = 1.0f / math::Abs((p0.x - p1.x) * (p2.y - p1.y) - (p0.y - p1.y) * (p2.x - p1.x));

    int samples = render_texture_->sample_size();
    for (int x = bound_min.x; x <= bound_max.x; ++x) {
        for (int y = bound_min.y; y <= bound_max.y; ++y) {
            Vec2i pixel(x, y);
            int mask = 0;
            for (int i = 0; i < samples; ++i) {
                Vec2f pos = render_texture_->GetSubSample(x, y, i);
                if (Resolve(pos, pixel, i, v0, v1, v2, area2_reciprocal)) {
                    mask |= (1 << i);
                }
            }

            if (mask != 0) {
                float fx = x + 0.5f;
                float fy = y + 0.5f;

                float e0 = EdgeFunction(fx, fy, p1, p2);
                float e1 = EdgeFunction(fx, fy, p2, p0);
                float e2 = EdgeFunction(fx, fy, p0, p1);

                float alpha = e0 * area2_reciprocal;
                float beta = e1 * area2_reciprocal;
                float gamma = e2 * area2_reciprocal;

                //depth in view space reciprocal (z)
                float w_reciprocal = 1.0f / (alpha * v0.w_reciprocal + beta * v1.w_reciprocal + gamma * v2.w_reciprocal);
                float w0 = alpha * v0.w_reciprocal * w_reciprocal;
                float w1 = beta * v1.w_reciprocal * w_reciprocal;
                float w2 = gamma * v2.w_reciprocal * w_reciprocal;

                VertexOut o = RasterizeLerp(v0, v1, v2, w0, w1, w2);
                
                Vec4f color = shader_->Frag(o);
                for (int i = 0; i < samples; ++i) {
                    if ((mask & (1 << i)) != 0) {
                        render_texture_->SetColor(x, y, color, i);
                    }
                }
            }
        }
    }
}

bool Graphics::Resolve(const Vec2f& pos, const Vec2i& pixel, int sub_sample, const VertexOut& v0, const VertexOut& v1, const VertexOut& v2,
        float area2_reciprocal) const {

    Vec3f w;
    if (!InsideTriangle(pos, v0, v1, v2, area2_reciprocal, w)) {
        return false;
    }

    //depth in NDC [-1,1]
    float z_interpolated = v0.position.z * w[0] + v1.position.z * w[1] + v2.position.z * w[2];
    if (z_interpolated >= render_texture_->GetDepth(pixel.x, pixel.y, sub_sample)) {
        return false;
    }

    if (write_depth_) {
        render_texture_->SetDepth(pixel.x, pixel.y, z_interpolated, sub_sample);
    }

    return true;
}

// Bresenham's line drawing algorithm
void Graphics::DrawLine(const Vec4f& begin, const Vec4f& end, const Vec4f& line_color) {
    assert(render_texture_);

    auto x0 = math::Clamp((int)(begin.x), 0, render_texture_->width() - 1);
    auto y0 = math::Clamp((int)(begin.y), 0, render_texture_->height() - 1);
    auto x1 = math::Clamp((int)(end.x), 0, render_texture_->width() - 1);
    auto y1 = math::Clamp((int)(end.y), 0, render_texture_->height() - 1);

    bool steep = math::Abs(y1 - y0) > math::Abs(x1 - x0);
    if (steep) {
        std::swap(x0, y0);
        std::swap(x1, y1);
    }

    if (x0 > x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    int dx = x1 - x0;
    int dy = math::Abs(y1 - y0);
    int err = dx / 2;    
    int ystep = y0 < y1 ? 1 : -1;
    int y = y0;
    if (steep) {
        for (int x = x0; x <= x1; ++x) {
            render_texture_->SetColor(y, x, line_color);
            err -= dy;
            if (err < 0) {
                y += ystep;
                err += dx;
            }
        }
    } else {
        for (int x = x0; x <= x1; ++x) {
            render_texture_->SetColor(x, y, line_color);
            err -= dy;
            if (err < 0) {
                y += ystep;
                err += dx;
            }
        }
    }
    
}

}
