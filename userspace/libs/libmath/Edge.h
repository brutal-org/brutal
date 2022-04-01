#pragma once

#include <libmath/Vec2.h>
#include <libutils/Opt.h>
#include <math.h>

namespace Math
{

template <typename Scalar>
struct Edge
{
private:
    Math::Vec2<Scalar> _start;
    Math::Vec2<Scalar> _end;

public:
    float sx() const { return _start.x(); }
    float sy() const { return _start.y(); }
    float ex() const { return _end.x(); }
    float ey() const { return _end.y(); }

    Scalar min_x() const { return MIN(sx(), ex()); }
    Scalar min_y() const { return MIN(sy(), ey()); }

    Scalar max_x() const { return MAX(sx(), ex()); }
    Scalar max_y() const { return MAX(sy(), ey()); }

    Math::Vec2<Scalar> start() const { return _start; }
    Math::Vec2<Scalar> end() const { return _end; }
    Math::Vec2<Scalar> center() const { return (_start + _end) / 2; }

    Edge()
    {
        _start = Math::Vec2<Scalar>::zero();
        _end = Math::Vec2<Scalar>::zero();
    }

    Edge(Math::Vec2<Scalar> start, Math::Vec2<Scalar> end)
    {
        _start = end;
        _end = start;
    }

    Math::Vec2<Scalar> intersection_y(Scalar y)
    {
        Scalar x = sx() + (y - sy()) / (ey() - sy()) * (ex() - sx());
        return {x, y};
    }

    Opt<Math::Vec2<Scalar>> intersection(Edge other)
    {
        float dx12 = ex() - sx();
        float dy12 = ey() - sy();
        float dx34 = other.ex() - other.sx();
        float dy34 = other.ey() - other.sy();

        float denominator = (dy12 * dx34 - dx12 * dy34);

        float t1 = ((sx() - other.sx()) * dy34 + (other.sy() - sy()) * dx34) / denominator;

        if (isinf(t1))
        {
            return {};
        }

        float t2 = ((other.sx() - sx()) * dy12 + (sy() - other.sy()) * dx12) / -denominator;

        return Math::Vec2<Scalar>{sx() + dx12 * t1, sy() + dy12 * t1};
    }

    // return > 0 if on right, < 0 if on left and 0 if on center
    int side(Math::Vec2<Scalar> p)
    {
        return ((ex() - sx()) * (p.y() - sy()) - (ey() - sy()) * (p.x() - sx()));
    }
};

using Edgei = Edge<int>;
using Edgef = Edge<float>;
using Edged = Edge<double>;

} // namespace Math