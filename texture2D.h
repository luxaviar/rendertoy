#pragma once

#include <vector>
#include <string>
#include <assert.h>
#include "math/vec2.h"
#include "math/vec3.h"
#include "math/mat4.h"
#include "uncopyable.h"
#include "buffer.h"
#include "types.h"

namespace rendertoy {

class Texture2D : private Uncopyable {
public:
    Texture2D();
    Texture2D(const char* filename, bool sRGB=false, TextureWrapMode mode=TextureWrapMode::kClamp);
    Texture2D(float* data, const Vec2i& offset, int image_width, int width, int height, int origin_channel, bool sRGB=false); //for HDR
    Texture2D(Texture2D&& other) = default;
    Texture2D& operator =(Texture2D&& other) = default;
    void Swap(Texture2D& other) noexcept;

    bool valid() const { return texture_.size() > 0; }
    const std::string& filename() const { return file_name_; }
    int origin_channel() const { return origin_channel_; }

    TextureWrapMode warp_mode() const { return mode_; }
    void warp_mode(TextureWrapMode mode) { mode_ = mode; }

    int height() const { return height_; }
    int width() const { return width_; }

    const Buffer<Vec4f>& texture() const { return texture_; }

    Vec4f GetColor(const Vec2i& p) const;    
    Vec4f Sample2D(Vec2f coord) const;
    Vec4f Sample2D(float u, float v) const;
    Vec3f SampleRGB(Vec2f coord) const;

    void ConvertToImage(Buffer<Col3U8>& image_buffer) const;
private:
    float Warp(float u) const;

    std::string file_name_;
    TextureWrapMode mode_;

    int width_;
    int height_;
    int origin_channel_;

    Buffer<Vec4f> texture_;
};

}
