#pragma once

#include <vector>
#include <assert.h>
#include "math/vec3.h"
#include "math/mat4.h"
#include "uncopyable.h"
#include "buffer.h"
#include "texture2D.h"

namespace rendertoy {

class RenderTexture : private Uncopyable{
public:
    RenderTexture(int w, int h);

    int height() const { return height_; }
    int width() const { return width_; }

    const Buffer<Vec4f>& color_buffer() const { return color_buffer_; }
    const Buffer<float>& depth_buffer() const { return depth_buffer_; }

    void SetColor(int x, int y, const Vec4f& color);
    void SetColor(int x, int y, const Vec3f& color);
    void SetDepth(int x, int y, float depth);

    Vec4f GetColor(int x, int y) const;
    float GetDepth(int x, int y) const;

    void Clear(Buffers buff, const Vec3f& color = { 49.0f / 255.0f, 77.0f / 255.0f,121.0f / 255.0f});

    void ConvertToImage(Buffer<Col3U8>& image_buffer);

private:
    int width_;
    int height_;

    Buffer<Vec4f> color_buffer_;
    Buffer<float> depth_buffer_;
};

}
