#pragma once

#include <math.h>

namespace rendertoy {
namespace math {
    //constexpr float kEpsilon = std::numeric_limits<float>::epsilon();
    constexpr float kEpsilon = 0.00001f;
    constexpr float kEpsilonSq = kEpsilon * kEpsilon;
    constexpr float kPI = 3.14159265358979323f;
    constexpr float kInvPI = 1.0f / kPI;
    constexpr float kDeg2Rad = kPI / 180.f;
    constexpr float kRad2Deg = 180.f / kPI;

    template<typename T>
    T Sign(T v) {
        if (v >= 0) return 1;
        return -1;
    }

    inline float Sign(float v) {
        return Sign<float>(v);
    }

    template<typename T>
    T Abs(T v) {
        if (v < 0) v = -v;
        return v;
    }

    inline float Abs(float v) {
        return Abs<float>(v);
    }

    template<typename T>
    T Max(T a, T b) {
        return a > b ? a : b;
    }

    inline float Max(float a, float b) {
        return Max<float>(a, b);
    }

    template<typename T>
    T Min(T a, T b) {
        return a > b ? b : a;
    }

    inline float Min(float a, float b) {
        return Min<float>(a, b);
    }

    template<typename T>
    T Pow(T a, T b) {
        return pow(a, b);
    }

    inline float Pow(float a, float b) {
        return Pow<float>(a, b);
    }

    template<typename T>
    T Clamp(T v, T min, T max) {
        if (v < min) return min;
        else if (v > max) return max;
        return v;
    }

    inline float Clamp(float v, float min, float max) {
        return Clamp<float>(v, min, max);
    }

    inline float Saturate(float v) {
        return Clamp<float>(v, 0.0f, 1.0f);
    }

    inline float Sqrt(float v) {
        return sqrt(v);
    }

    inline double Sqrt(double v) {
        return sqrt(v);
    }

    inline bool AlmostEqual(float left, float right, float epsilon = kEpsilon) {
        return Abs(left - right) < epsilon;
    }

    inline bool AlmostEqual(double left, double right, double epsilon = kEpsilon) {
        return Abs(left - right) < epsilon;
    }

    inline float Sin(float angle) {
        return sin(angle);
    }

    inline float Asin(float angle) {
        return asin(angle);
    }

    inline float Cos(float angle) {
        return cos(angle);
    }

    inline float Acos(float angle) {
        return acos(angle);
    }

    inline float Atan(float v) {
        return atan(v);
    }

    inline float Atan2(float y, float x) {
        return atan2(y, x);
    }

    inline float Tan(float angle) {
        return tan(angle);
    }

    // [0, 1] -> [0, 1 << n - 1]
    template<typename I>
    inline I QuantizedUnormEncode(float v) {
        size_t n = sizeof(I) * 8;
        return (I)(v * ((1 << n) - 1) + 0.5f);
    }

    // [0, 1 << n - 1] -> [0, 1]
    template<typename I>
    inline float QuantizedUnormDecode(I v) {
        size_t n = sizeof(I) * 8;
        return (float)v / ((1 << n) - 1);
    }
}

}

