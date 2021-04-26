#pragma once

#include "tred/embedded_types.h"
#include "tred/types.h"

enum class TextureEdge
{
    Clamp, Repeat
};


enum class TextureRenderStyle
{
    Pixel, Smooth
};


enum class Transperency
{
    Include, Exclude
};

struct LoadedImage
{
    unsigned int id;
    int width; int height;
};

LoadedImage
LoadImageEmbeded
(
    const EmbeddedBinary& image_binary,
    TextureEdge texture_edge,
    TextureRenderStyle texture_render_style,
    Transperency transperency
);

LoadedImage
LoadImageFromPixels
(
    void* pixels, int width, int height,
    TextureEdge texture_edge,
    TextureRenderStyle texture_render_style,
    Transperency transperency
);

LoadedImage
LoadImageSingleFromSinglePixel
(
    u32 pixel,
    TextureEdge texture_edge,
    TextureRenderStyle texture_render_style,
    Transperency transperency
);
