#include "rendertexture.h"
#include <cmath>
#include <cassert>
#include "math/util.h"
#include "common/color.h"
#include "image.h"

namespace rendertoy {

Texture2D::Texture2D() {

}

Texture2D::Texture2D(const char* filename, bool sRGB, TextureWrapMode mode) : 
    file_name_(filename),
    mode_(mode),
    origin_channel_(0)
{
    int width, height;
    uint8_t* data = image_load(filename, &width, &height, &origin_channel_, 4);
    assert (data);

    texture_.Resize(width, height);
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            uint8_t* pixel = data + (width * i + j) * 4;
            if (sRGB) {
                Vec3f color(pixel[0] / 255.0f, pixel[1] / 255.0f, pixel[2] / 255.0f);
                color = GammaToLinearSpace(color);
                texture_.Set(j, i, { color, pixel[3] / 255.0f });
            } else {
                texture_.Set(j, i, { pixel[0] / 255.0f, pixel[1] / 255.0f, pixel[2] / 255.0f, pixel[3] / 255.0f });
            }
        }
    }
    image_free(data);
}

Texture2D::Texture2D(float* data, const Vec2i& offset, int image_width, int width, int height, int origin_channel, bool sRGB) : 
    mode_(TextureWrapMode::kClamp),
    origin_channel_(origin_channel)
{
    texture_.Resize(width, height);
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            int x = offset.x + j;
            int y = offset.y + i;
            float* pixel = data + (image_width * y + x) * 4;
            if (sRGB) {
                Vec3f color(pixel[0], pixel[1], pixel[2]);
                color = GammaToLinearSpace(color);
                texture_.Set(j, i, { color, pixel[3] });
            } else {
                texture_.Set(j, i, { pixel[0], pixel[1], pixel[2], pixel[3] });
            }
        }
    }
}

void Texture2D::Swap(Texture2D& other) noexcept {
    std::swap(mode_, other.mode_);
    std::swap(origin_channel_, other.origin_channel_);
    std::swap(file_name_, other.file_name_);
    texture_.Swap(other.texture_);
}

float Texture2D::Warp(float u) const {
    if (mode_ == TextureWrapMode::kClamp) {
        u = math::Clamp(u, 0.0f, 1.0f);
    } else {
        u -= std::floor(u);
    }

    return u;
}

Vec3f Texture2D::SampleRGB(Vec2f coord) const {
    Vec4f color = Sample2D(coord.u, coord.v);
    return { color.r, color.g, color.b };
}

Vec4f Texture2D::Sample2D(Vec2f coord) const {
    return Sample2D(coord.u, coord.v);
}

Vec4f Texture2D::Sample2D(float u, float v) const {
    if (u < 0.0f || u > 1.0f) {
        u = Warp(u);
    }

    if (v < 0.0f || v > 1.0f) {
        v = Warp(v);
    }

    return texture_.Sample(u, v);
}

void Texture2D::ConvertToImage(Buffer<Col3U8>& image_buffer) const {
    assert(image_buffer.width() == texture_.width());
    assert(image_buffer.height() == texture_.height());

    for (int i = 0; i< texture_.height(); ++i) {
        for (int j = 0;j < texture_.width(); ++j) {
            auto& col = texture_.Get(j, i);
            image_buffer.Set(j, texture_.height() - i - 1, Col3U8(col.r * 255.0f, col.g * 255.0f, col.b * 255.0f));
        }
    }
}

}
