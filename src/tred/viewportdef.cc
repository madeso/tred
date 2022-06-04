#include "tred/viewportdef.h"

#include "tred/assert.h"

#define ASSERTX(X,...) ASSERT(X)


ViewportDef
ViewportDef::fit_with_black_bars
(
    float width,
    float height,
    int window_width,
    int window_height
)
{
    ASSERTX(width > 0, width);
    ASSERTX(height > 0, height);
    ASSERTX(window_width >= 0, window_width);
    ASSERTX(window_height >= 0, window_height);
    const float w = static_cast<float>(window_width) / width;
    const float h = static_cast<float>(window_height) / height;
    const float s = std::min(w, h);
    ASSERTX(s > 0, s, w, h);
    const float new_width = width * s;
    const float new_height = height * s;

    return ViewportDef
    {
        Recti(static_cast<int>(new_width), static_cast<int>(new_height)).set_bottom_left
        (
            static_cast<int>((static_cast<float>(window_width) - new_width) / 2.0f),
            static_cast<int>((static_cast<float>(window_height) - new_height) / 2.0f)
        ),
        width,
        height
    };
}


float
DetermineExtendScale(float scale, float height, int window_height)
{
    const auto scaled_height = height * scale;
    const auto s = static_cast<float>(window_height) / scaled_height;
    return s;
}


ViewportDef
ViewportDef::extend
(
    float width,
    float height,
    int window_width,
    int window_height
)
{
    ASSERTX(width >= 0, width);
    ASSERTX(height >= 0, height);
    ASSERTX(window_width >= 0, window_width);
    ASSERTX(window_height >= 0, window_height);
    const auto w = static_cast<float>(window_width) / width;
    const auto h = static_cast<float>(window_height) / height;
    const auto r = Recti(window_width, window_height)
                            .set_bottom_left(0, 0);
    if(w < h)
    {
        const auto s = DetermineExtendScale(w, height, window_height);
        return ViewportDef {r, width, height * s};
    }
    else
    {
        const auto s = DetermineExtendScale(h, width, window_width);
        return ViewportDef {r, width * s, height};
    }
}


ViewportDef
ViewportDef::screen_pixel(int window_width, int window_height)
{
    ASSERTX(window_width >= 0, window_width);
    ASSERTX(window_height >= 0, window_height);

    return ViewportDef
    {
        Recti(window_width, window_height)
                .set_bottom_left(0, 0),
        static_cast<float>(window_width),
        static_cast<float>(window_height)
    };
}


ViewportDef::ViewportDef(const Recti& screen, float w, float h)
    : screen_rect(screen)
    , virtual_width(w)
    , virtual_height(h)
{
}



namespace
{
    float lerp(float lhs, float t, float rhs)
    {
        return lhs + t * (rhs - lhs);
    }

    int lerp(int lhs, float t, int rhs)
    {
        return static_cast<int>
        (
            lerp
            (
                static_cast<float>(lhs),
                t,
                static_cast<float>(rhs)
            )
        );
    }

    template<typename T>
    Rect<T> lerp(const Rect<T>& lhs, float t, const Rect<T>& rhs)
    {
        #define V(x) lerp(lhs.x, t, rhs.x)
        return
        {
            V(left),
            V(bottom),
            V(right),
            V(top)
        };
        #undef V
    }
}


[[nodiscard]]
ViewportDef
lerp
(
    const ViewportDef& lhs,
    float t,
    const ViewportDef& rhs
)
{
    #define V(x) lerp(lhs.x, t, rhs.x)
    return
    {
        V(screen_rect),
        V(virtual_width),
        V(virtual_height)
    };
    #undef V
}
