#pragma once

#include "embed/types.h"
#include "tred/types.h"

enum class TextureEdge
{
    clamp, repeat
};


enum class TextureRenderStyle
{
    pixel, smooth
};


enum class Transparency
{
    include, exclude
};

struct LoadedImage
{
    unsigned int id;
    int width; int height;
};

LoadedImage
load_image_from_embedded
(
    const embedded_binary& image_binary,
    TextureEdge te,
    TextureRenderStyle trs,
    Transparency t
);

LoadedImage
load_image_from_pixel_buffer
(
    void* pixels, int width, int height,
    TextureEdge te,
    TextureRenderStyle trs,
    Transparency t
);

LoadedImage
load_image_from_color
(
    u32 pixel,
    TextureEdge te,
    TextureRenderStyle trs,
    Transparency t
);
