#pragma once

#include "tred/rect.h"

struct ViewportDef
{
    recti screen_rect;

    float virtual_width;
    float virtual_height;

    /** The viewport is scaled, with aspect in mind, and centered.
     * Fits the viewport, scaling it, keeping the aspect ratio. Black bars may appear if the aspect ration doesnt match
     */
    [[nodiscard]]
    static
    ViewportDef
    FitWithBlackBars
    (
        float width,
        float height,
        int window_width,
        int window_height
    );

    /** The viewports height or idth is extended to match the screen.
     * Fits the viewport, scaling it to the max and then fits the viewport without stretching. This means that the viewport isnt the same size as requested.
     */
    [[nodiscard]]
    static
    ViewportDef
    Extend(float width, float height, int window_width, int window_height);

    /** The viewport matches the screen pixel by pixel.
     */
    [[nodiscard]]
    static
    ViewportDef
    ScreenPixel(int window_width, int window_height);

    ViewportDef(const recti& screen, float w, float h);
};
