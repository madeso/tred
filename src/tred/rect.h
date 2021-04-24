#pragma once

// basic rect
// also heavily inspired by https://halt.software/dead-simple-layouts/

struct rect
{
    float minx;
    float miny;
    float maxx;
    float maxy;

    constexpr rect
    (
          float mix
        , float miy
        , float max
        , float may
    )
        : minx(mix)
        , miny(miy)
        , maxx(max)
        , maxy(may)
    {
    }

    constexpr rect
    (
          float width
        , float height
    )
        : minx(0.0f)
        , miny(0.0f)
        , maxx(width)
        , maxy(height)
    {
    }

    constexpr static rect from_xywh
    (
        float x, float y, float w, float h
    )
    {
        return {x, y, x+w, y+h};
    }
};


rect cut_left(::rect* rect, float a);
rect cut_right(::rect* rect, float a);
rect cut_top(::rect* rect, float a);
rect cut_bottom(::rect* rect, float a);

enum class side {
    left,
    right,
    top,
    bottom,
};

struct rect_cut
{
    ::rect* rect;
    side side;
};


rect rectcut_cut(const rect_cut& rectcut, float a);

// Extensions
#if 0

// Same as cut, except they keep the input rect intact.
// Useful for decorations (9-patch-much?).
rect get_left(const ::rect* rect, float a);
rect get_right(const ::rect* rect, float a);
rect get_top(const ::rect* rect, float a);
rect get_bottom(const ::rect* rect, float a);

// These will add a rectangle outside of the input rectangle.
// Useful for tooltips and other overlay elements.
rect add_left(const ::rect* rect, float a);
rect add_right(const ::rect* rect, float a);
rect add_top(const ::rect* rect, float a);
rect add_bottom(const ::rect* rect, float a);

#endif
