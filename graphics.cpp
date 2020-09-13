#include "graphics.h"
#include <algorithm>
#include <vector>
#include <math.h>
#include <assert.h>
#include "shader/shader.h"
#include "camera.h"

namespace rendertoy {

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

bool Graphics::Cull(const VertexOut& v0, const VertexOut& v1, const VertexOut& v2) {
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
        pos.x = 0.5 * width * (pos.x + 1.0);
        pos.y = 0.5 * height * (pos.y + 1.0);
        pos.z = 0.5 * (pos.z + 1.0);
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
            RasterizeTriangle(o0, o1, o2, is_front);
        }
    }

    clip_output_.clear();
}

void Graphics::RasterizeTriangle(const VertexOut& v0, const VertexOut& v1, const VertexOut& v2, bool is_front) {
    Vec4f p0 = v0.position;
    Vec4f p1 = v1.position;
    Vec4f p2 = v2.position;
    
    int x0 = (int)(p0.x);
    int y0 = (int)(p0.y);
    int x1 = (int)(p1.x);
    int y1 = (int)(p1.y);
    int x2 = (int)(p2.x);
    int y2 = (int)(p2.y);

    Vec2i bound_min;
    Vec2i bound_max;
    bound_min.x = (std::min)((std::min)((std::min)(x0, x1), x2), render_texture_->width() - 1);
    bound_min.y = (std::min)((std::min)((std::min)(y0, y1), y2), render_texture_->height() - 1);

    bound_max.x = (std::min)((std::max)((std::max)(x0, x1), x2), render_texture_->width() - 1);
    bound_max.y = (std::min)((std::max)((std::max)(y0, y1), y2), render_texture_->height() - 1);

    float area2_reciprocal = 1.0f / math::Abs((p0.x - p1.x) * (p2.y - p1.y) - (p0.y - p1.y) * (p2.x - p1.x));

    for (auto x = bound_min.x; x <= bound_max.x; ++x) {
        for (auto y = bound_min.y; y <= bound_max.y; ++y) {
            float e0 = is_front ? EdgeFunction(x, y, p1, p2) : EdgeFunction(x, y, p2, p1);
            if (e0 < 0) continue;

            float e1 = is_front ? EdgeFunction(x, y, p2, p0) : EdgeFunction(x, y, p0, p2);
            if (e1 < 0) continue;

            float e2 = is_front ? EdgeFunction(x, y, p0, p1) : EdgeFunction(x, y, p1, p0);
            if (e2 < 0) continue;

            float alpha = e0 * area2_reciprocal;
            float beta = e1 * area2_reciprocal;
            float gamma = e2 * area2_reciprocal;

            //depth in view space reciprocal (z)
            float w_reciprocal = 1.0f / (alpha * v0.w_reciprocal + beta * v1.w_reciprocal + gamma * v2.w_reciprocal);
            float w0 = alpha * v0.w_reciprocal * w_reciprocal;
            float w1 = beta * v1.w_reciprocal * w_reciprocal;
            float w2 = gamma * v2.w_reciprocal * w_reciprocal;

            //depth in NDC [-1,1]
            float z_interpolated = p0.z * w0 + p1.z * w1 + p2.z * w2;
            if (z_interpolated < render_texture_->GetDepth(x, y)) {
                if (write_depth_) {
                    render_texture_->SetDepth(x, y, z_interpolated);
                }
                VertexOut o = RasterizeLerp(v0, v1, v2, w0, w1, w2);
                Vec4f color = shader_->Frag(o);
                render_texture_->SetColor(x, y, color);
            }
        }
    }
}

// Bresenham's line drawing algorithm
void Graphics::DrawLine(const Vec4f& begin, const Vec4f& end, const Vec4f& line_color) {
    assert(render_texture_);

    auto x1 = math::Clamp((int)(begin.x + 0.5f), 0, render_texture_->width() - 1);
    auto y1 = math::Clamp((int)(begin.y + 0.5f), 0, render_texture_->height() - 1);
    auto x2 = math::Clamp((int)(end.x + 0.5f), 0, render_texture_->width() - 1);
    auto y2 = math::Clamp((int)(end.y + 0.5f), 0, render_texture_->height() - 1);

    int x, y, dx, dy, dx1, dy1, px, py, xe, ye, i;

    dx = x2 - x1;
    dy = y2 - y1;
    dx1 = fabs(dx);
    dy1 = fabs(dy);
    px = 2 * dy1 - dx1;
    py = 2 * dx1 - dy1;

    if(dy1 <= dx1) {
        if(dx >= 0) {
            x = x1;
            y = y1;
            xe = x2;
        } else {
            x = x2;
            y = y2;
            xe = x1;
        }

        render_texture_->SetColor(x, y, line_color);
        
        for(i = 0; x < xe; i++) {
            x = x + 1;
            if (px < 0) {
                px = px + 2 * dy1;
            } else {
                if((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) {
                    y = y + 1;
                } else {
                    y = y - 1;
                }
                px = px + 2 * (dy1 - dx1);
            }

            render_texture_->SetColor(x, y, line_color);
        }
    } else {
        if(dy >= 0) {
            x = x1;
            y = y1;
            ye = y2;
        } else {
            x = x2;
            y = y2;
            ye = y1;
        }
        
        render_texture_->SetColor(x, y, line_color);
        for(i = 0; y < ye; i++) {
            y = y + 1;
            if(py <= 0) {
                py = py + 2 * dx1;
            } else {
                if((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) {
                    x = x + 1;
                } else {
                    x = x - 1;
                }
                py = py + 2 * (dx1 - dy1);
            }
            render_texture_->SetColor(x, y, line_color);
        }
    }
}

}
