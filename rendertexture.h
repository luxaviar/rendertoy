#pragma once

#include <vector>
#include <assert.h>
#include "math/vec3.h"
#include "math/mat4.h"
#include "uncopyable.h"
#include "buffer.h"
#include "texture2D.h"
#include "types.h"

namespace rendertoy {

class RenderTexture : private Uncopyable{
public:
    RenderTexture(int w, int h, MSAALevel lvl = MSAALevel::kNone);

    int height() const { return height_; }
    int width() const { return width_; }

    const Buffer<Vec4f>& color_buffer() const { return color_buffer_; }
    const Buffer<float>& depth_buffer() const { return depth_buffer_; }

    const MSAALevel msaa() const { return msaa_; }
    void msaa(MSAALevel lvl);
    const int sample_size() const { return sample_size_; }

    Vec2f GetSubSample(int x, int y, int sub_sample);

    void SetColor(int x, int y, const Vec4f& color);
    void SetDepth(int x, int y, float depth);

    void SetColor(int x, int y, const Vec4f& color, int sub_sample);
    void SetDepth(int x, int y, float depth, int sub_sample);
    
    Vec4f GetColor(int x, int y) const;
    float GetDepth(int x, int y) const;

    Vec4f GetColor(int x, int y, int sub_sample) const;
    float GetDepth(int x, int y, int sub_sample) const;

    void Clear(Buffers buff, const Vec3f& color = { 49.0f / 255.0f, 77.0f / 255.0f,121.0f / 255.0f});

    void ConvertToImage(Buffer<Col3U8>& image_buffer);

private:
    //https://docs.microsoft.com/zh-cn/windows/win32/api/d3d11/ne-d3d11-d3d11_standard_multisample_quality_levels?redirectedfrom=MSDN
    static constexpr Vec2f kMSAAPattern0[] = {
        {0.0f, 0.0f}
    };

    static constexpr Vec2f kMSAAPattern2[] = {
        {-0.25f, 0.25f},
        {0.25f, -0.25f}
    };

    static constexpr Vec2f kMSAAPattern4[] = {
        {0.375f, -0.125f},
        {0.125f, 0.375f},
        {-0.375f, 0.125f},
        {-0.125f, -0.375f}
    };

    MSAALevel msaa_;
    const Vec2f* msaa_pattern_;
    int sample_exp_;
    int sample_size_;    

    int width_;
    int height_;

    Buffer<Vec4f> color_buffer_;
    Buffer<float> depth_buffer_;
};

}
