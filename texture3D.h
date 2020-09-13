#pragma once

#include <array>
#include "texture2D.h"

namespace rendertoy {

class Texture3D : private Uncopyable {
public:
    Texture3D(const char* filename);
    Texture3D(Texture3D&& other) = default;

    const std::array<Texture2D, 6>& faces() const { return faces_; }

    Vec4f Sample3D(Vec3f coord) const;
    Vec3f SampleRGB(Vec3f coord) const;

private:
    std::array<Texture2D, 6> faces_;
};

}
