#include "tred/rect.h"

#include <algorithm>
#include <cassert>

rect cut_left(::rect* rect, float a)
{
    float minx = rect->minx;
    rect->minx = std::min(rect->maxx, rect->minx + a);
    return { minx, rect->miny, rect->minx, rect->maxy };
}

rect cut_right(::rect* rect, float a)
{
    float maxx = rect->maxx;
    rect->maxx = std::max(rect->minx, rect->maxx - a);
    return { rect->maxx, rect->miny, maxx, rect->maxy };
}

rect cut_top(::rect* rect, float a)
{
    float miny = rect->miny;
    rect->miny = std::min(rect->maxy, rect->miny + a);
    return { rect->minx, miny, rect->maxx, rect->miny };
}

rect cut_bottom(::rect* rect, float a)
{
    float maxy = rect->maxy;
    rect->maxy = std::max(rect->miny, rect->maxy - a);
    return { rect->minx, rect->maxy, rect->maxx, maxy };
}

rect rectcut_cut(const rect_cut& rectcut, float a)
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
