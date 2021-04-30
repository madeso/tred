#pragma once

#include <algorithm>
#include <cassert>

#include "glm/glm.hpp"

// basic rect
// also heavily inspired by https://halt.software/dead-simple-layouts/

template<typename T>
struct trect
{
    T minx;
    T miny;
    T maxx;
    T maxy;

    constexpr trect<T>
    (
          T mix
        , T miy
        , T max
        , T may
    )
        : minx(mix)
        , miny(miy)
        , maxx(max)
        , maxy(may)
    {
        assert(minx <= maxx && miny <= maxy);
    }

    constexpr trect<T>
    (
          T width
        , T height
    )
        : minx(0)
        , miny(0)
        , maxx(width)
        , maxy(height)
    {
        assert(width >= 0 && height >= 0);
    }

    constexpr trect<T>
    zero() const
    {
        return {0, 0, maxx - minx, maxy - miny};
    }

    constexpr trect<T>
    scale_uniform(T s) const
    {
        return {minx * s, miny * s, maxx * s, maxy * s};
    }

    constexpr bool
    is_inside_inclusive(T x, T y) const
    {
        return x >= minx && x <= maxx
            && y >= miny && y <= maxy;
    }

    // todo(Gustav): replace with a specific function call
    template<typename Y>
    constexpr trect<Y>
    cast() const
    {
        return {static_cast<Y>(minx), static_cast<Y>(miny), static_cast<Y>(maxx), static_cast<Y>(maxy)};
    }

    constexpr trect<T>
    translate(T dx, T dy) const
    {
        return {minx + dx, miny + dy, maxx + dx, maxy + dy};
    }

    template<glm::qualifier Q>
    constexpr trect<T>
    translate(const glm::vec<2, T, Q>& v) const
    {
        return translate(v.x, v.y);
    }

    template<glm::qualifier Q>
    constexpr glm::vec<2, T, Q>
    to01(const glm::vec<2, T, Q>& p) const
    {
        return
        {
            (p.x - minx) / get_width(),
            (p.y - miny) / get_height()
        };
    }

    template<glm::qualifier Q>
    constexpr glm::vec<2, T, Q>
    from01(const glm::vec<2, T, Q>& p) const
    {
        return
        {
            minx + (p.x * get_width()),
            miny + (p.y * get_height())
        };
    }

    constexpr T
    get_height() const
    {
        return maxy - miny;
    }

    constexpr T
    get_width() const
    {
        return maxx - minx;
    }

    constexpr T
    get_scale_for_height(T new_height) const
    {
        const T old_height = get_height();
        return new_height / old_height;
    }

    constexpr T
    get_scale_for_width(T new_width) const
    {
        const T old_width = get_width();
        return new_width / old_width;
    }

    constexpr trect<T>
    set_height(T new_height) const
    {
        return scale_uniform(get_scale_for_height(new_height));
    }

    constexpr static trect<T>
    from_xywh(T x, T y, T w, T h)
    {
        return {x, y, x+w, y+h};
    }

    constexpr trect<T>
    set_bottom_left(T x, T y)
    {
        return zero().translate(x, y);
    }


    // These will add a rectangle outside of the input rectangle.
    // Useful for tooltips and other overlay elements.
    constexpr trect<T> add_left(T a) const
    {
        return {minx-a, miny, maxx, maxy};
    }

    constexpr trect<T> add_right(T a) const
    {
        return {minx, miny, maxx+a, maxy};
    }

    constexpr trect<T> add_top(T a) const
    {
        return {minx, miny, maxx, maxy+a};
    }

    constexpr trect<T> add_bottom(T a) const
    {
        return {minx, miny-a, maxx, maxy};
    }

    constexpr trect<T> extend(T a) const
    {
        return add_left(a).add_right(a).add_top(a).add_bottom(a);
    }

    constexpr trect<T> cut_to_center(T w, T h) const
    {
        const auto dx = (get_width() - w)/2;
        const auto dy = (get_height() - h)/2;
        const auto x = minx+dx;
        const auto y = miny+dy;
        return {x,y, x+w, y+h};
    }
};


enum class side
{
    left,
    right,
    top,
    bottom,
};

template<typename T>
struct rect_cut
{
    ::trect<T>* rect;
    side side;
};

template<typename T>
trect<T>
cut_left(trect<T>* rect, T a)
{
    T minx = rect->minx;
    rect->minx = std::min(rect->maxx, rect->minx + a);
    return { minx, rect->miny, rect->minx, rect->maxy };
}

template<typename T>
trect<T>
cut_right(trect<T>* rect, T a)
{
    T maxx = rect->maxx;
    rect->maxx = std::max(rect->minx, rect->maxx - a);
    return { rect->maxx, rect->miny, maxx, rect->maxy };
}

template<typename T>
trect<T>
cut_top(trect<T>* rect, T a)
{
    T miny = rect->miny;
    rect->miny = std::min(rect->maxy, rect->miny + a);
    return { rect->minx, miny, rect->maxx, rect->miny };
}

template<typename T>
trect<T>
cut_bottom(trect<T>* rect, T a)
{
    T maxy = rect->maxy;
    rect->maxy = std::max(rect->miny, rect->maxy - a);
    return { rect->minx, rect->maxy, rect->maxx, maxy };
}

template<typename T>
trect<T>
rectcut_cut(const rect_cut<T>& rectcut, T a)
{
    switch (rectcut.side)
    {
        case side::left:   return cut_left(rectcut.rect,   a);
        case side::right:  return cut_right(rectcut.rect,  a);
        case side::top:    return cut_top(rectcut.rect,    a);
        case side::bottom: return cut_bottom(rectcut.rect, a);
        default:
            assert(false);
            return cut_bottom(rectcut.rect, a);
    }
}



// Extensions
#if 0

// Same as cut, except they keep the input rect intact.
// Useful for decorations (9-patch-much?).
rect get_left(const trect<T>& rect, T a);
rect get_right(const trect<T>& rect, T a);
rect get_top(const trect<T>& rect, T a);
rect get_bottom(const trect<T>& rect, T a);

#endif


using rect = trect<float>;
using recti = trect<int>;

constexpr rect
Cint_to_float(const recti& r)
{
    return
    {
        static_cast<float>(r.minx),
        static_cast<float>(r.miny),
        static_cast<float>(r.maxx),
        static_cast<float>(r.maxy)
    };
}
