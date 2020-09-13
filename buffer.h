#pragma once

#include <vector>
#include <assert.h>
#include "uncopyable.h"

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

    void resize(int w, int h) {
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

    void Fill(const T& value) {
        std::fill(data_.begin(), data_.end(), value);
    }

private:
    std::vector<T> data_;
    int width_;
    int height_;
};

}
