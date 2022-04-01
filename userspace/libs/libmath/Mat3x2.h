#pragma once

#include <libmath/CubicBezier.h>
#include <libmath/Edge.h>
#include <libmath/Vec2.h>

#include <math.h>
#include <string.h>

namespace Math
{

template <typename Scalar>
struct Mat3x2
{
private:
    Scalar _m[6] = {};

public:
    static ALWAYS_INLINE Mat3x2 identity()
    {
        return {
            1.0f,
            0.0f,
            0.0f,
            1.0f,
            0.0f,
            0.0f,
        };
    }

    static ALWAYS_INLINE Mat3x2 translation(Vec2<Scalar> vec)
    {
        return {
            1.0f,
            0.0f,
            0.0f,
            1.0f,
            vec.x(),
            vec.y(),
        };
    }

    static ALWAYS_INLINE Mat3x2 scale(Scalar s) { return scale({s, s}); }

    static ALWAYS_INLINE Mat3x2 scale(const Vec2<Scalar> &s)
    {
        return {
            s.x(),
            0.0f,
            0.0f,
            s.y(),
            0,
            0,
        };
    }

    static ALWAYS_INLINE Mat3x2 skewX(Scalar a)
    {
        return {
            1.0f,
            0.0f,
            tanf(a),
            1.0f,
            0.0f,
            0.0f,
        };
    }

    static ALWAYS_INLINE Mat3x2 skewY(Scalar a)
    {
        return {
            1.0f,
            tanf(a),
            0.0f,
            1.0f,
            0.0f,
            0.0f,
        };
    }

    static ALWAYS_INLINE Mat3x2 rotation(Scalar a)
    {
        Scalar cs = cosf(a);
        Scalar sn = sinf(a);

        return {
            cs,
            sn,
            -sn,
            cs,
            0.0f,
            0.0f,
        };
    }

    ALWAYS_INLINE Scalar operator[](size_t i) const
    {
        assert(i < 6);
        return _m[i];
    }

    ALWAYS_INLINE Mat3x2()
    {
    }

    ALWAYS_INLINE Mat3x2(const Scalar m[6])
    {
        memcpy(_m, m, sizeof(Scalar) * 6);
    }

    ALWAYS_INLINE Mat3x2(Scalar m0, Scalar m1, Scalar m2, Scalar m3, Scalar m4, Scalar m5)
        : _m{
              m0,
              m1,
              m2,
              m3,
              m4,
              m5,
          }
    {
    }

    ALWAYS_INLINE Scalar determinant() const
    {
        return _m[0] * _m[3] - _m[2] * _m[1];
    }

    ALWAYS_INLINE Mat3x2 inverse() const
    {
        auto det = determinant();

        if (det > -1e-6 && det < 1e-6)
        {
            return identity();
        }

        auto invdet = 1 / det;

        return {
            _m[3] * invdet,
            -_m[2] * invdet,
            (_m[2] * _m[5] - _m[3] * _m[4]) * invdet,
            -_m[1] * invdet,
            _m[0] * invdet,
            (_m[1] * _m[4] - _m[0] * _m[5]) * invdet,
        };
    }

    ALWAYS_INLINE CubicBezier<Scalar> apply(const CubicBezier<Scalar> &b) const
    {
        return {
            apply(b.start()),
            apply(b.cp1()),
            apply(b.cp2()),
            apply(b.end()),
        };
    }

    ALWAYS_INLINE CubicBezier<Scalar> apply(const Edge<Scalar> &e) const
    {
        return {
            apply(e.start()),
            apply(e.end()),
        };
    }

    ALWAYS_INLINE Vec2<Scalar> apply(const Vec2<Scalar> &v) const
    {
        return {
            v.x() * _m[0] + v.y() * _m[2] + _m[4],
            v.x() * _m[1] + v.y() * _m[3] + _m[5],
        };
    }

    ALWAYS_INLINE Vec2<Scalar> apply_no_translation(const Vec2<Scalar> &v) const
    {
        return {
            v.x() * _m[0] + v.y() * _m[2],
            v.x() * _m[1] + v.y() * _m[3],
        };
    }

    ALWAYS_INLINE Mat3x2 operator*(const Mat3x2 &other) const
    {
        return {
            _m[0] * other[0] + _m[1] * other[2],
            _m[0] * other[1] + _m[1] * other[3],
            _m[2] * other[0] + _m[3] * other[2],
            _m[2] * other[1] + _m[3] * other[3],
            _m[4] * other[1] + _m[5] * other[3] + other[5],
            _m[4] * other[0] + _m[5] * other[2] + other[4],
        };
    }
};

using Mat3x2i = Mat3x2<int>;
using Mat3x2f = Mat3x2<float>;
using Mat3x2d = Mat3x2<double>;

} // namespace Math