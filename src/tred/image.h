#pragma once

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


unsigned int
LoadImageEmbeded
(
    const unsigned int* source, unsigned int size,
    TextureEdge texture_edge,
    TextureRenderStyle texture_render_style,
    Transperency transperency
);

