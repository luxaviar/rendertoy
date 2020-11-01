#pragma once

#include <vector>
#include "common/singleton.h"
#include "rendertexture.h"
#include "vertex.h"
#include "types.h"
#include "screen_triangle.h"

namespace rendertoy {

class Shader;
struct Camera;

class Graphics : public Singleton<Graphics> {
public:
    void SetRenderTarget(RenderTexture* rt);
    void SetShader(const Shader* shader);
    void SetClipDistance(float near, float far);
    void SetRenderType(Primitive type);

    void SetWriteDepth(bool on);
    void SetWriteColor(bool on);
    void SetCullMode(CullMode mode);

    void DrawLine(const Vec4f& begin, const Vec4f& end, const Vec4f& line_color);
    void DrawTriangle(const Vertex& v0, const Vertex& v1, const Vertex& v2);

protected:
    Graphics();

private:
    static constexpr ClipPlane kClipPlanes[] = {
        {math::Axis::kX, 1.0f},
        {math::Axis::kX, -1.0f},
        {math::Axis::kY, 1.0f},
        {math::Axis::kY, -1.0f},
        {math::Axis::kZ, 1.0f},
        {math::Axis::kZ, -1.0f}
    };
        
    static VertexOut RasterizeLerp(const VertexOut& v0, const VertexOut& v1, float w);
    static VertexOut Lerp(const VertexOut& v0, const VertexOut& v1, float w);

    void RasterizeEdgeEquation(const VertexOut& v0, const VertexOut& v1, const VertexOut& v2);
    void RasterizePixel(const ScreenTriangle& tri, int x, int y);

    void RasterizeEdgeWalking(const VertexOut& v0, const VertexOut& v1, const VertexOut& v2);
    void RasterizeFlatTriangle(const VertexOut* v0, const VertexOut* v1, const VertexOut* v2);

    void Clip(const VertexOut& v0, const VertexOut& v1, const VertexOut& v2, std::vector<VertexOut>& result);
    
    std::vector<VertexOut> clip_output_;
    std::vector<VertexOut> clip_input_;

    float near_;
    float far_;

    bool write_depth_;
    bool write_color_;
    CullMode cull_;

    const Shader* shader_;
    RenderTexture* render_texture_;
    Primitive render_type_;
};

}
