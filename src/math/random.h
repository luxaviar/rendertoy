#pragma once

#include <cstdint>
#include "rand.h"
#include "vec2.h"
#include "vec3.h"
#include "quat.h"
#include "util.h"

extern rendertoy::math::Rand g_rng;

namespace rendertoy {
namespace math {
namespace random {

inline void SetSeed(uint32_t sd) {
    g_rng.SetSeed(sd);
}

inline uint32_t Value() {
    return g_rng.Get();
}

inline float Range01() {
    return g_rng.GetFloat();
}

inline float Range(float min, float max) {
    float t = g_rng.GetFloat();
    t = min * t + (1.0f - t) * max;
    return t;
}

inline int Range(int min, int max) {
    int dif;
    if (min < max) {
        dif = max - min;
        int t = g_rng.Get() % dif;
        t += min;
        return t;
    } else if (min > max) {
        dif = min - max;
        int t = g_rng.Get() % dif;
        t = min - t;
        return t;
    } else {
        return min;
    }        
}

inline Vec3f UnitVector() {
    float z = Range(-1.0f, 1.0f);
    float a = Range(0.0f, 2.0f * kPI);

    float r = Sqrt (1.0f - z*z);

    float x = r * Cos (a);
    float y = r * Sin (a);

    return Vec3f(x, y, z);
}

inline Vec2f UnitVec2f() {
    float a = Range(0.0f, 2.0f * kPI);

    float x = Cos(a);
    float y = Sin(a);

    return Vec2f(x, y);
}

inline rendertoy::Quaternion Quaternion() {
    rendertoy::Quaternion q;
    q.x = Range(-1.0f, 1.0f);
    q.y = Range(-1.0f, 1.0f);
    q.z = Range(-1.0f, 1.0f);
    q.w = Range(-1.0f, 1.0f);
    q.Normalized();
    if (q.Dot(rendertoy::Quaternion::identity) < 0.0f)
        return -q;
    else
        return q;
}

inline Vec3f PointInsideUnitSphere() {
    Vec3f v = UnitVector();
    v *= Pow(Range01(), 1.0f / 3.0f);
    return v;
}

inline Vec2f PointInsideUnitCircle() {
    Vec2f v = UnitVec2f();
    // As the volume of the sphere increases (x^3) over an interval we have to increase range as well with x^(1/3)
    v *= Pow(Range(0.0f, 1.0f), 1.0f / 2.0f);
    return v;
}

}
}
}