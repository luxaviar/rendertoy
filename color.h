#pragma once

#include <cmath>
#include "math/vec3.h"

namespace rendertoy {

inline float GammaToLinearSpaceExact (float value) {
    if (value <= 0.04045f)
        return value / 12.92f;
    else if (value < 1.0f)
        return std::pow((value + 0.055f)/1.055f, 2.4f);
    else
        return std::pow(value, 2.2f);
}

inline Vec3f GammaToLinearSpace (const Vec3f& sRGB) {
    // Approximate version from http://chilliant.blogspot.com.au/2012/08/srgb-approximations-for-hlsl.html?m=1
    return sRGB * (sRGB * (sRGB * 0.305306011f + 0.682171111f) + 0.012522878f);

    // Precise version, useful for debugging.
    //return Vec3f(GammaToLinearSpaceExact(sRGB.r), GammaToLinearSpaceExact(sRGB.g), GammaToLinearSpaceExact(sRGB.b));
}

inline float LinearToGammaSpaceExact (float value) {
    if (value <= 0.0f)
        return 0.0f;
    else if (value <= 0.0031308F)
        return 12.92f * value;
    else if (value < 1.0F)
        return 1.055f * std::pow(value, 0.4166667f) - 0.055f;
    else
        return std::pow(value, 0.45454545F);
}

inline Vec3f LinearToGammaSpace (Vec3f linRGB) {
    linRGB = Vec3f::Max(linRGB, Vec3f::zero);
    // An almost-perfect approximation from http://chilliant.blogspot.com.au/2012/08/srgb-approximations-for-hlsl.html?m=1
    return Vec3f::Max(1.055f * Vec3f::Pow(linRGB, 0.416666667f) - 0.055f, Vec3f::zero);
    
    // Exact version, useful for debugging.
    //return half3(LinearToGammaSpaceExact(linRGB.r), LinearToGammaSpaceExact(linRGB.g), LinearToGammaSpaceExact(linRGB.b))
}

}
