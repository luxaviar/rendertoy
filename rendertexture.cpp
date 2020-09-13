#include "rendertexture.h"
#include  <cmath>

namespace rendertoy {

RenderTexture::RenderTexture(int w, int h) : 
    width_(w), 
    height_(h),
    color_buffer_(w, h),
    depth_buffer_(w, h)
{
    
}

Vec4f RenderTexture::GetColor(int x, int y) const {
    return color_buffer_.Get(x, y);
}

float RenderTexture::GetDepth(int x, int y) const {
    return depth_buffer_.Get(x, y);
}

void RenderTexture::SetColor(int x, int y, const Vec4f& color) {
    color_buffer_.Set(x, y, color);
}

void RenderTexture::SetColor(int x, int y, const Vec3f& color) {
    color_buffer_.Set(x, y, Vec4f(color.r, color.g, color.b, 1.0f));
}

void RenderTexture::SetDepth(int x, int y, float depth) {
    depth_buffer_.Set(x, y, depth);
}

void RenderTexture::Clear(Buffers buff, const Vec3f& color) {
    if ((buff & Buffers::kColor) == Buffers::kColor) {
        Vec4f col(color.r, color.g, color.b, 1.0f);
        color_buffer_.Fill(col);
    }

    if ((buff & Buffers::kDepth) == Buffers::kDepth) {
        depth_buffer_.Fill(std::numeric_limits<float>::infinity());
    }
}

void RenderTexture::ConvertToImage(Buffer<Col3U8>& image_buffer) {
    assert(image_buffer.width() == width_);
    assert(image_buffer.height() == height_);

    for (int i = 0; i< height_; ++i) {
        for (int j = 0;j < width_; ++j) {
            auto& f32c = GetColor(j, i);
            image_buffer.Set(j, height_ - i - 1, 
                Col3U8(math::Saturate(f32c.r) * 255.0f, math::Saturate(f32c.g) * 255.0f, math::Saturate(f32c.b) * 255.0f));
        }
    }
}

}
