#pragma once

#include <vector>
#include "singleton.h"
#include "triangle.h"
#include "rendertexture.h"
#include "vertex.h"
#include "types.h"

namespace rendertoy {

class Shader;
struct Camera;

class Graphics : public Singleton<Graphics> {
public:
    void SetRenderTarget(RenderTexture* rt);
    void SetShader(const Shader* shader);
    void SetCamera(const Camera* camera);
    void SetRenderType(Primitive type);

    void SetWriteDepth(bool on);
    void SetCullMode(CullMode mode);

    void DrawLine(const Vec4f& begin, const Vec4f& end, const Vec4f& line_color);
    void DrawTriangle(const Vertex& v0, const Vertex& v1, const Vertex& v2);

protected:
    Graphics() : near_(0.1f), far_(50.0f), write_depth_(true), cull_(CullMode::kBack), shader_(nullptr), render_texture_(nullptr), render_type_(Primitive::kLine) {}

private:
    static constexpr ClipPlane kClipPlanes[] = {
        {math::Axis::kX, 1.0f},
        {math::Axis::kX, -1.0f},
        {math::Axis::kY, 1.0f},
        {math::Axis::kY, -1.0f},
        {math::Axis::kZ, 1.0f},
        {math::Axis::kZ, -1.0f}
    };

    float EdgeFunction(float x, float y, const Vec4f& v0, const Vec4f& v1);
    void RasterizeTriangle(const VertexOut& v0, const VertexOut& v1, const VertexOut& v2, bool is_front);
    VertexOut RasterizeLerp(const VertexOut& v0, const VertexOut& v1, const VertexOut& v2, float w0, float w1, float w2);
    VertexOut Lerp(const VertexOut& v0, const VertexOut& v1, float w);

    bool Cull(const VertexOut& v0, const VertexOut& v1, const VertexOut& v2);
    void Clip(const VertexOut& v0, const VertexOut& v1, const VertexOut& v2, std::vector<VertexOut>& result);
    
    std::vector<VertexOut> clip_output_;
    std::vector<VertexOut> clip_input_;

    float near_;
    float far_;

    bool write_depth_;
    CullMode cull_;

    const Shader* shader_;
    RenderTexture* render_texture_;
    Primitive render_type_;
};

}
