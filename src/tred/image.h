#pragma once

#include "tred/embedded_types.h"

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

