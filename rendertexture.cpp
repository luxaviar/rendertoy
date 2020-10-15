#include "rendertexture.h"
#include  <cmath>
#include "color.h"

namespace rendertoy {

RenderTexture::RenderTexture(int w, int h, MSAALevel lvl) : 
    width_(w), 
    height_(h)
{
    msaa(lvl);
}

void RenderTexture::msaa(MSAALevel lvl) {
    msaa_ = lvl;
    sample_exp_ = static_cast<int>(lvl);
    sample_size_ = std::pow(2, sample_exp_);
    if (msaa_ == MSAALevel::k2x) {
        msaa_pattern_ = kMSAAPattern2;
    } else if (msaa_ == MSAALevel::k4x) {
        msaa_pattern_ = kMSAAPattern4;
    } else {
        msaa_pattern_ = kMSAAPattern0;
    }

    color_buffer_.Resize(width_ * sample_size_, height_);
    depth_buffer_.Resize(width_ * sample_size_, height_);
}

Vec2f RenderTexture::GetSubSample(int x, int y, int sub_sample) {
    assert(sub_sample < sample_size_);
    const Vec2f& offset = msaa_pattern_[sub_sample];
    return Vec2f(x + 0.5f + offset.x, y + 0.5f + offset.y);
}

Vec4f RenderTexture::GetColor(int x, int y) const {
    Vec4f color;
    for (int i = 0; i < sample_size_; ++i) {
        color += GetColor(x, y, i);
    }
    color /= sample_size_;
    return color;
}

float RenderTexture::GetDepth(int x, int y) const {
    float depth = 0.0f;
    for (int i = 0; i < sample_size_; ++i) {
        depth += GetDepth(x, y, i);
    }
    depth /= sample_size_;
    return depth;
}

Vec4f RenderTexture::GetColor(int x, int y, int sub_sample) const {
    assert(sub_sample < sample_size_);
    return color_buffer_.Get((x << sample_exp_) + sub_sample, y);
}

float RenderTexture::GetDepth(int x, int y, int sub_sample) const {
    assert(sub_sample < sample_size_);
    return depth_buffer_.Get((x << sample_exp_) + sub_sample, y);
}

void RenderTexture::SetColor(int x, int y, const Vec4f& color) {
    for (int i = 0; i < sample_size_; ++i) {
        SetColor(x, y, color, i);
    }
}

void RenderTexture::SetDepth(int x, int y, float depth) {
    for (int i = 0; i < sample_size_; ++i) {
        SetDepth(x, y, depth, i);
    }
}

void RenderTexture::SetColor(int x, int y, const Vec4f& color, int sub_sample) {
    assert(sub_sample < sample_size_);
    color_buffer_.Set((x << sample_exp_) + sub_sample, y, color);
}

void RenderTexture::SetDepth(int x, int y, float depth, int sub_sample) {
    assert(sub_sample < sample_size_);
    depth_buffer_.Set((x << sample_exp_) + sub_sample, y, depth);
}

void RenderTexture::Clear(Buffers buff, const Vec3f& color) {
    if ((buff & Buffers::kColor) == Buffers::kColor) {
        Vec3f linear_color = GammaToLinearSpace(color);
        Vec4f col(linear_color.r, linear_color.g, linear_color.b, 1.0f);
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
            Vec4f col = GetColor(j, i);
            Vec3f color(col.r, col.g, col.b);

            // HDR tonemapping
            // https://knarkowicz.wordpress.com/2016/01/06/aces-filmic-tone-mapping-curve/
            color = ACESToneMapping(color);

            color = LinearToGammaSpace(color);

            image_buffer.Set(j, height_ - i - 1, Col3U8(math::Saturate(color.r) * 255, 
                math::Saturate(color.g) * 255, math::Saturate(color.b) * 255));
        }
    }
}

}
