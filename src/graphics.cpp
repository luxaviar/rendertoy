#include "graphics.h"
#include <algorithm>
#include <vector>
#include <cmath>
#include <cassert>
#include "shader/shader.h"
#include "camera.h"
#include "math/util.h"

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
    SetWriteColor(shader_->write_color());
    SetCullMode(shader_->cull());
}

void Graphics::SetClipDistance(float near, float far) {
    near_ = near;
    far_ = far;
}

void Graphics::SetRenderType(Primitive type) {
    render_type_ = type;
}

void Graphics::SetWriteDepth(bool on) {
    write_depth_ = on;
}

void Graphics::SetWriteColor(bool on) {
    write_color_ = on;
}

void Graphics::SetCullMode(CullMode mode) { 
    cull_ = mode;
}

VertexOut Graphics::Lerp(const VertexOut& v0, const VertexOut& v1, float w) {
    VertexOut o;
    o.w_reciprocal = math::Lerp(v0.w_reciprocal, v1.w_reciprocal, w);
    o.position = Vec4f::Lerp(v0.position, v1.position, w);
    o.world_position = Vec3f::Lerp(v0.world_position, v1.world_position, w);
    o.color = Vec4f::Lerp(v0.color, v1.color, w);
    o.normal = Vec3f::Lerp(v0.normal, v1.normal, w);
    o.tangent = Vec3f::Lerp(v0.tangent, v1.tangent, w);
    o.texcoord = Vec2f::Lerp(v0.texcoord, v1.texcoord, w);
    o.shadow_coord = Vec3f::Lerp(v0.shadow_coord, v1.shadow_coord, w);
    
    return o;
}

VertexOut Graphics::RasterizeLerp(const VertexOut& v0, const VertexOut& v1, float w) {
    VertexOut o;
    o.w_reciprocal = math::Lerp(v0.w_reciprocal, v1.w_reciprocal, w);
    o.position = Vec4f::Lerp(v0.position, v1.position, w);
    o.world_position = Vec3f::Lerp(v0.world_position * v0.w_reciprocal, v1.world_position * v1.w_reciprocal, w) / o.w_reciprocal;
    o.color = Vec4f::Lerp(v0.color * v0.w_reciprocal, v1.color * v1.w_reciprocal, w) / o.w_reciprocal;
    o.normal = Vec3f::Lerp(v0.normal * v0.w_reciprocal, v1.normal * v1.w_reciprocal, w) / o.w_reciprocal;
    o.tangent = Vec3f::Lerp(v0.tangent * v0.w_reciprocal, v1.tangent * v1.w_reciprocal, w) / o.w_reciprocal;
    o.texcoord = Vec2f::Lerp(v0.texcoord * v0.w_reciprocal, v1.texcoord * v1.w_reciprocal, w) / o.w_reciprocal;
    o.shadow_coord = Vec3f::Lerp(v0.shadow_coord * v0.w_reciprocal, v1.shadow_coord * v1.w_reciprocal, w) / o.w_reciprocal;

    return o;
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
        
        bool is_front = (o1.position.y - o0.position.y) * (o2.position.x - o0.position.x) -
            (o1.position.x - o0.position.x) * (o2.position.y - o0.position.y) > 0.0f;

        if ((cull_ == CullMode::kBack && !is_front) ||
            (cull_ == CullMode::kFront && is_front))
            break;

        if (render_type_ == Primitive::kLine) {
            DrawLine(o0.position, o1.position, Vec4f(0.0f, 1.0f, 0.0f, 1.0f));
            DrawLine(o1.position, o2.position, Vec4f(0.0f, 1.0f, 0.0f, 1.0f));
            DrawLine(o2.position, o0.position, Vec4f(0.0f, 1.0f, 0.0f, 1.0f));
        } else {
            if (is_front) {
                RasterizeEdgeEquation(o0, o1, o2);
            } else {
                RasterizeEdgeEquation(o0, o2, o1);
            }
        }
    }

    clip_output_.clear();
}

void Graphics::RasterizeEdgeEquation(const VertexOut& v0, const VertexOut& v1, const VertexOut& v2) {
    ScreenTriangle tri(v0, v1, v2, render_texture_);
    for (int x = tri.min.x; x <= tri.max.x; ++x) {
        for (int y = tri.min.y; y <= tri.max.y; ++y) {
            RasterizePixel(tri, x, y);
        }
    }
}

void Graphics::RasterizePixel(const ScreenTriangle& tri, int x, int y) {
    Vec2i pixel(x, y);
    int mask = 0;
    int samples = render_texture_->sample_size();
    for (int i = 0; i < samples; ++i) {
        Vec2f pos = render_texture_->GetSubSample(x, y, i);
        float depth;
        if (tri.Coverage(pos, pixel, i, depth)) {
            mask |= (1 << i);
            if (write_depth_) {
                render_texture_->SetDepth(x, y, depth, i);
            }
        }
    }
    if (mask == 0) return;

    if (write_color_) {
        VertexOut o = tri.Rasterize(x + 0.5f, y + 0.5f);
        Vec4f color = shader_->Frag(o);
        for (int i = 0; i < samples; ++i) {
            if ((mask & (1 << i)) != 0) {
                render_texture_->SetColor(x, y, color, i);
            }
        }
    }
}

void Graphics::RasterizeEdgeWalking(const VertexOut& v0, const VertexOut& v1, const VertexOut& v2) {
    const VertexOut* arr[] = {
        &v0, //top
        &v1, //middle
        &v2  //bottom
    }; 

    if (arr[0]->position.y < arr[1]->position.y) {
        std::swap(arr[0], arr[1]);
    }

    if (arr[0]->position.y < arr[2]->position.y) {
        std::swap(arr[0], arr[2]);
    }

    if (arr[1]->position.y < arr[2]->position.y) {
        std::swap(arr[1], arr[2]);
    }

    if ((int)arr[0]->position.y == (int)arr[1]->position.y) {//top flat
        RasterizeFlatTriangle(arr[0], arr[1], arr[2]);
    } else if ((int)arr[1]->position.y == (int)arr[2]->position.y) { //bottom flat
        RasterizeFlatTriangle(arr[1], arr[2], arr[0]);
    } else {
        float w = (arr[0]->position.y - arr[1]->position.y) / (arr[0]->position.y - arr[2]->position.y);
        VertexOut breakpoint = RasterizeLerp(*arr[0], *arr[2], w);
        RasterizeFlatTriangle(&breakpoint, arr[1], arr[0]);
        RasterizeFlatTriangle(&breakpoint, arr[1], arr[2]);
    }
}

void Graphics::RasterizeFlatTriangle(const VertexOut* v0, const VertexOut* v1, const VertexOut* v2) {
    const VertexOut* left = v0;
    const VertexOut* right = v1;

    if (v0->position.x > v1->position.x) {
        std::swap(left, right);
        std::swap(v0, v1);
    }

    bool going_down = true;
    // make sure cw-wise order
    if (v2->position.y > v0->position.y) {
        std::swap(v0, v1);
        going_down = false;
    }

    int lo = (int)math::Min(left->position.y, right->position.y);
    int hi = v2->position.y;
    int height = math::Abs(hi - lo);

    float k_left = (v2->position.x - left->position.x) / (v2->position.y - left->position.y);
    float k_right = (v2->position.x - right->position.x) / (v2->position.y - right->position.y);

    int step = 1;
    int adj_left = left->position.x < v2->position.x ? -1 : 1;
    int adj_right = right->position.x < v2->position.x ? 1 : -1;
    if (going_down) {
        k_left = -k_left;
        k_right = -k_right;
        step = -1;
        adj_left = -adj_left;
        adj_right = -adj_right;
    }

    float ly_min = left->position.y;
    float ry_min = right->position.y;
    float ly_max = v2->position.y;
    float ry_max = v2->position.y;

    if (ly_max < ly_min) {
        std::swap(ly_min, ly_max);
    }

    if (ry_max < ry_min) {
        std::swap(ry_min, ry_max);
    }

    ScreenTriangle tri(*v0, *v1, *v2, render_texture_);
    int y = lo - step;
    for (int i = 0; i <= height; ++i) {
        y += step;
        if (y < tri.min.y || y > tri.max.y) continue;

        float ly = math::Clamp(y + adj_left, ly_min, ly_max);
        float ry = math::Clamp(y + adj_right, ry_min, ry_max);
        float ld = ly - left->position.y;
        float rd = ry - right->position.y;
        int lx = left->position.x + (ld > 0 ? 1 : -1) * k_left * ld;
        int rx = right->position.x + (rd > 0 ? 1 : -1) * k_right * rd;
        
        for (int x = lx; x <= rx; ++x) {
            if (x < tri.min.x || x > tri.max.x) continue;;
            RasterizePixel(tri, x, y);
        }
    }
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
