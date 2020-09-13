#pragma once

namespace rendertoy {

using Col3U8 = math::Vec3<uint8_t>;
using Col4U8 = math::Vec4<uint8_t>;

enum class TextureWrapMode {
    kClamp,
    kRepeat,
};

enum class CubeFace : uint8_t {
    kFront = 0,
    kBack,
    kLeft,
    kRight,
    kTop,
    kBottom,
};

enum class CullMode : uint8_t {
    kNone,
    kBack,
    kFront,
};

enum class Primitive {
    kLine,
    kTriangle
};

}