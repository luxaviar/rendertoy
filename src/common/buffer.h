#pragma once

#include <vector>
#include <assert.h>
#include "common/uncopyable.h"
#include "math/vec2.h"
#include "math/util.h"

namespace rendertoy {

enum class Buffers {
    kColor = 1,
    kDepth = 2
};

inline Buffers operator|(Buffers a, Buffers b) {
    return Buffers((int)a | (int)b);
}

inline Buffers operator&(Buffers a, Buffers b) {
    return Buffers((int)a & (int)b);
}

template <typename T>
class Buffer : private Uncopyable {
public:
    Buffer(int width, int height) : width_(width), height_(height), data_(width * height) {
        // data_.resize(width * height);
    }

    Buffer() : width_(0), height_(0) {

    }

    Buffer(Buffer<T>&& other) = default;
    Buffer<T>& operator=(Buffer&& other) = default;

    void Swap(Buffer<T>& other) noexcept {
        std::swap(width_, other.width_);
        std::swap(height_, other.height_);
        data_.swap(other.data_);
    }

    std::vector<T>& data() { return data_;  }
    const std::vector<T>& data() const { return data_; }

    size_t size() const { return data_.size(); }

    int width() const { return width_; }
    int height() const { return height_; }

    void Resize(int w, int h) {
        width_ = w;
        height_ = h;
        data_.resize(w * h);
    }

    void Set(int w, int h, const T& value) {
        assert(h >= 0 && h < height_);
        assert(w >= 0 && w < width_);
        data_[width_ * h + w] = value;
    }

    T Get(int w, int h) const {
        assert(h >= 0 && h < height_);
        assert(w >= 0 && w < width_);
        return data_[width_ * h + w];
    }

    T Get(const Vec2i& coord) const {
        return Get(coord.x, coord.y);
    }
    
    void Fill(const T& value) {
        std::fill(data_.begin(), data_.end(), value);
    }

    T Sample(float u, float v) const {
        u *= width_;
        v *= height_;

        Vec2i u00(std::floor(u), std::ceil(v));
        u00.x = math::Clamp(u00.x, 0, width_ - 1);
        u00.y = math::Clamp(u00.y, 0, height_ - 1);

        Vec2i u10(std::ceil(u), std::ceil(v));
        u10.x = math::Clamp(u10.x, 0, width_ - 1);
        u10.y = math::Clamp(u10.y, 0, height_ - 1);

        Vec2i u01(std::floor(u), std::floor(v));
        u01.x = math::Clamp(u01.x, 0, width_ - 1);
        u01.y = math::Clamp(u01.y, 0, height_ - 1);

        Vec2i u11(std::ceil(u), std::floor(v));
        u11.x = math::Clamp(u11.x, 0, width_ - 1);
        u11.y = math::Clamp(u11.y, 0, height_ - 1);

        float s = u - std::floor(u);
        float t = std::ceil(v) - v;

        T bc = Get(u00) + (Get(u10) - Get(u00)) * s;
        T tc = Get(u01) + (Get(u11) - Get(u01)) * s;

        return bc + (tc - bc) * t;
    }

private:
    std::vector<T> data_;
    int width_;
    int height_;
};

}