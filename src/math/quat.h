#pragma once

#include <assert.h>
#include "util.h"
#include "mat3.h"

namespace rendertoy {
namespace math {

template<typename T>
struct Quat {
    typedef T value_type;
    static constexpr int kSize = 4;
    static const Quat<T> identity;

    union {
        T value[kSize];
        struct {
            T x, y, z, w;
        };
    };

    constexpr Quat() : x(0), y(0), z(0), w(0) {}

    constexpr Quat(T x_, T y_, T z_, T w_) : x(x_), y(y_), z(z_), w(w_) {
    }

    constexpr Quat(T v) : x(v), y(v), z(v), w(v) {
    }

    constexpr Quat(const Quat<T>& v) : x(v.x), y(v.y), z(v.z), w(v.w) {
    }

    void Set(T x, T y, T z, T w) {
        value[0] = x;
        value[1] = y;
        value[2] = z;
        value[3] = w;
    }

    void Zero() {
        x = 0;
        y = 0;
        z = 0;
        w = 0;
    }

    Quat<T>& Abs() {
        x = math::Abs(x);
        y = math::Abs(y);
        z = math::Abs(z);
        w = math::Abs(w);
        return *this;
    }

    Quat<T>& Normalized() {
        T mag = Magnitude();
        if (mag > kEpsilon) {
            T inv = (T)1.0f / mag;
            x *= inv;
            y *= inv;
            z *= inv;
            w *= inv;
        } else {
            x = y = z = 0.0f;
            w = 1.0f;
        }
        return *this;
    }

    Quat<T> Normalize() const {
        T mag = Magnitude();
        if (mag > kEpsilon) {
            T inv = (T)1.0f / mag;
            return Quat<T>(x * inv, y * inv, z * inv, w * inv);
        }

        return Quat<T>(0.0f, 0.0f, 0.0f, 1.0f);
    }

    Quat<T>& Inverse() {
        x = -x;
        y = -y;
        z = -z;
        return *this;
    }

    Quat<T> Inversed() const {
        return Quat<T>(-x, -y, -z, w);
    }

    T MagnitudeSq() const {
        return x * x + y * y + z * z + w * w;
    }

    T Magnitude() const {
        return Sqrt(MagnitudeSq());
    }

    T Dot(const Quat<T>& v) const {
        return x * v.x + y * v.y + z * v.z + w * v.w;
    }

    void Scale(T v) {
        Normalized();
        x *= v;
        y *= v;
        z *= v;
        w *= v;
    }

    void Limit(T v) {
        T mag = Magnitude();
        if (mag > v) {
            T inv = (T)1.0f / mag * v;
            x = x * inv;
            y = y * inv;
            z = z * inv;
            w = w * inv;
        }
    }

    bool AlmostEquals(const Quat<T>& v) const {
        return Dot(v) > (T)0.999998986721039;
    }

    T operator[](int index) const {
        return value[index];
    }

    T& operator[](int index) {
        return value[index];
    }

    Quat<T> operator +() const {
        return Quat<T>(x, y, z, w);
    }

    Quat<T> operator +(const Quat<T>& v) const {
        return Quat<T>(x + v.x, y + v.y, z + v.z, w + v.w);
    }

    Quat<T> operator +(T v) const {
        return Quat<T>(x + v, y + v, z + v, z + w);
    }

    Quat<T>& operator +=(T v) {
        x += v;
        y += v;
        z += v;
        w += v;
        return *this;
    }

    Quat<T>& operator +=(const Quat<T>& v) {
        x += v.x;
        y += v.y;
        z += v.z;
        w += v.z;
        return *this;
    }

    Quat<T> operator -(const Quat<T>& v) const {
        return Quat<T>(x - v.x, y - v.y, z - v.z, w - v.w);
    }

    Quat<T> operator -(T v) const {
        return Quat<T>(x - v, y - v, z - v, w - v);
    }

    Quat<T> operator -() const {
        return Quat<T>(-x, -y, -z, -w);
    }

    Quat<T>& operator -=(T v) {
        x -= v;
        y -= v;
        z -= v;
        w -= v;
        return *this;
    }

    Quat<T>& operator -=(const Quat<T>& v) {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        w -= v.w;
        return *this;
    }

    Quat<T> operator *(const Quat<T>& v) const {
        return Quat<T>(
                w * v.x + x * v.w + y * v.z - z * v.y,
                w * v.y + y * v.w + z * v.x - x * v.z,
                w * v.z + z * v.w + x * v.y - y * v.x,
                w * v.w - x * v.x - y * v.y - z * v.z
        );
    }

    Vec3<T> operator *(const Vec3<T>& v) const {
            float num1 = x * 2.0f;
            float num2 = y * 2.0f;
            float num3 = z * 2.0f;
            float num4 = x * num1;
            float num5 = y * num2;
            float num6 = z * num3;
            float num7 = x * num2;
            float num8 = x * num3;
            float num9 = y * num3;
            float num10 = w * num1;
            float num11 = w * num2;
            float num12 = w * num3;
            return Vec3<T>(
                (((T)1 - (num5 + num6)) * v.x + (num7 - num12) * v.y + (num8 + num11) * v.z),
                ((num7 + num12) * v.x + ((T)1 - (num4 + num6)) * v.y + (num9 - num10) * v.z),
                ((num8 - num11) * v.x + (num9 + num10) * v.y + ((T)1 - (num4 + num5)) * v.z)
            );
    }

    Quat<T> operator *(T v) const {
        return Quat<T>(x * v, y * v, z * v, w * v);
    }

    Quat<T>& operator *=(const Quat<T>& v) {
        x = w * v.x + x * v.w + y * v.z - z * v.y;
        y = w * v.y + y * v.w + z * v.x - x * v.z;
        z = w * v.z + z * v.w + x * v.y - y * v.x;
        w = w * v.w - x * v.x - y * v.y - z * v.z;
        return *this;
    }

    Quat<T> operator /(T v) const {
        T inv = (T)1 / v;
        return Quat<T>(x * inv, y * inv, z * inv, w * inv);
    }

    Quat<T>& operator /=(T v) {
        T inv = (T)1 / v;
        x *= inv;
        y *= inv;
        z *= inv;
        w *= inv;
        return *this;
    }

    bool operator ==(const Quat<T>& v) const {
        return x == v.x && y == v.y && z == v.z && w == v.w;
    }

    bool operator !=(const Quat<T>& v) const {
        return x != v.x || y != v.y || z != v.z || w != v.w;
    }

    static Quat<T> Min(const Quat<T>& left, const Quat<T>& right) {
        return Quat<T>(math::Min(left.x, right.x), math::Min(left.y, right.y), math::Min(left.z, right.z), math::Min(left.w, right.w));
    }

    static Quat<T> Max(const Quat<T>& left, const Quat<T>& right) {
        return Quat<T>(math::Max(left.x, right.x), math::Max(left.y, right.y), math::Max(left.z, right.z), math::Max(left.w, right.w));
    }

    static Quat<T> AngleAxis(float angle, const Vec3<T>& axis) {
        float halfAngle = angle * math::kDeg2Rad * 0.5f;
        float s = Sin(halfAngle);

        return Quat<T>(
            s * axis.x,
            s * axis.y,
            s * axis.z,
            Cos(halfAngle)
        );
    }

    static Quat<T> FromToRotation(const Vec3<T>& fromDirection, const Vec3<T>& toDirection) {
        T fromMag = fromDirection.Magnitude();
        T toMag = toDirection.Magnitude();

        if (fromMag < kEpsilon || toMag < kEpsilon) {
            return identity;
        }

        auto m = Mat3x3<T>::FromToRotation(fromDirection, toDirection);
        return FromMatrix(m);
    }

    static Quat<T> LookRotation(const Vec3<T>& forward) {
        return LookRotation(forward, Vec3<T>::up);
    }

    static Quat<T> LookRotation(const Vec3<T>& forward, const Vec3<T>& upward) {
        Mat3x3<T> m;
        if (!m.LookRotation(forward, upward)) {
            return identity;
        }

        return FromMatrix(m);
    }

    static Quat<T> Lerp(const Quat<T>& from, const Quat<T>& to, float t) {
        t = Clamp(t, 0.0f, 1.0f);
        Quat<T> tmpQuat;
        // if (dot < 0), q1 and q2 are more than 360 deg apart.
        // The problem is that quaternions are 720deg of freedom.
        // so we - all components when lerping
        if (from.Dot(to) < 0.0f) {
            tmpQuat.x = from.x + t * (-to.x - from.x);
            tmpQuat.y = from.y + t * (-to.y - from.y);
            tmpQuat.z = from.z + t * (-to.z - from.z);
            tmpQuat.w = from.w + t * (-to.w - from.w);
        }
        else {
            tmpQuat.x = from.x + t * (to.x - from.x);
            tmpQuat.y = from.y + t * (to.y - from.y);
            tmpQuat.z = from.z + t * (to.z - from.z);
            tmpQuat.w = from.w + t * (to.w - from.w);
        }

        tmpQuat.Normalize();
        return tmpQuat;
    }

    static Quat<T> Slerp(const Quat<T>& from, const Quat<T>& to, float t) {
        t = Clamp(t, 0.0f, 1.0f);

        T dot = from.Dot(to);

        // dot = cos(theta)
        // if (dot < 0), q1 and q2 are more than 90 degrees apart,
        // so we can invert one to reduce spinning
        Quat<T> tmpQuat;
        if (dot < 0.0f) {
            dot = -dot;
            tmpQuat.x = -to.x;
            tmpQuat.y = -to.y;
            tmpQuat.z = -to.z;
            tmpQuat.w = -to.w;
        } else {
            tmpQuat = to;
        }

        if (dot < 0.95f) {
            float angle = Acos(dot);
            float sinadiv, sinat, sinaomt;
            sinadiv = 1.0f / Sin(angle);
            sinat = Sin(angle * t);
            sinaomt = Sin(angle * (1.0f - t));
            tmpQuat.x = (from.x * sinaomt + tmpQuat.x * sinat) * sinadiv;
            tmpQuat.y = (from.y * sinaomt + tmpQuat.y * sinat) * sinadiv;
            tmpQuat.z = (from.z * sinaomt + tmpQuat.z * sinat) * sinadiv;
            tmpQuat.w = (from.w * sinaomt + tmpQuat.w * sinat) * sinadiv;
            //		AssertIf (!CompareApproximately (SqrMagnitude (tmpQuat), 1.0F));
            return tmpQuat;
        }
        // if the angle is small, use linear interpolation
        else {
            return Lerp(from, tmpQuat, t);
        }
    }

    Mat3x3<T> ToMatrix() const {
        T x = this->x + this->x;
        T y = this->y + this->y;
        T z = this->z + this->z;
        T xx = this->x * x;
        T yy = this->y * y;
        T zz = this->z * z;
        T xy = this->x * y;
        T xz = this->x * z;
        T yz = this->y * z;
        T wx = this->w * x;
        T wy = this->w * y;
        T wz = this->w * z;

        return Mat3x3<T>(
            1.0f - (yy + zz), xy - wz, xz + wy,
            xy + wz, 1.0f - (xx + zz), yz - wx,
            xz - wy, yz + wx, 1.0f - (xx + yy)
        );
    }

    static Quat<T> FromMatrix(const Mat3x3<T>& mat) {
        // Algorithm in Ken Shoemake's article in 1987 SIGGRAPH course notes
        // article "Quatf Calculus and Fast A
        T fTrace = mat(0, 0) + mat(1, 1) + mat(2, 2); //m[0][0] + m[1][1] + m[2][2];
        T fRoot;
        Quat<T> q;
        if (fTrace > 0.0f) {
            // |w| > 1/2, may as well choose w > 1/2
            fRoot = Sqrt(fTrace + 1.0f);  // 2w
            q.w = 0.5f * fRoot;
            fRoot = 0.5f / fRoot;  // 1/(4w)
            q.x = (mat(2, 1) - mat(1, 2)) * fRoot;
            q.y = (mat(0, 2) - mat(2, 0)) * fRoot;
            q.z = (mat(1, 0) - mat(0, 1)) * fRoot;
        } else {
            // |w| <= 1/2
            int s_iNext[] = { 1, 2, 0 };
            int i = 0;
            if (mat(1, 1) > mat(0, 0))
                i = 1;
            if (mat(2, 2) > mat(i, i))
                i = 2;
            int j = s_iNext[i];
            int k = s_iNext[j];

            fRoot = Sqrt(mat(i, i) - mat(j, j) - mat(k, k) + 1.0f);
            //float[] apkQuat = new float { &q.x, &q.y, &q.z };
            q[i] = 0.5f * fRoot;
            fRoot = 0.5f / fRoot;
            q.w = (mat(k, j) - mat(j, k)) * fRoot;
            q[j] = (mat(j, i) + mat(i, j)) * fRoot;
            q[k] = (mat(k, i) + mat(i, k)) * fRoot;
        }

        return q.Normalize();
    }

    template<typename I>
    Vec3<I> QuantizedEncode() const {
        Vec3<I> ret;
        if (w >= 0.0f) {
            ret.x = math::QuantizedUnormEncode<I>((x + 1.0f) / 2.0f);
            ret.y = math::QuantizedUnormEncode<I>((y + 1.0f) / 2.0f);
            ret.z = math::QuantizedUnormEncode<I>((z + 1.0f) / 2.0f);
        } else {
            ret.x = math::QuantizedUnormEncode<I>((-x + 1.0f) / 2.0f);
            ret.y = math::QuantizedUnormEncode<I>((-y + 1.0f) / 2.0f);
            ret.z = math::QuantizedUnormEncode<I>((-z + 1.0f) / 2.0f);
        }
        return ret;
    }

    template<typename I>
    static Quat<T> QuantizedDecode(Vec3<I> v) {
        Quat<T> rot;
        rot.x = math::QuantizedUnormDecode<I>(v.x) * 2.0f - 1.0f;
        rot.y = math::QuantizedUnormDecode<I>(v.y) * 2.0f - 1.0f;
        rot.z = math::QuantizedUnormDecode<I>(v.z) * 2.0f - 1.0f;
        rot.w = math::Sqrt(1.0f - (rot.x * rot.x + rot.y * rot.y + rot.z * rot.z));
        return rot;
    }
};

template<typename T>
const Quat<T> Quat<T>::identity((T)0, (T)0, (T)0, (T)1);

}

using Quaternion = math::Quat<float>;

}
