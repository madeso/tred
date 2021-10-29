#pragma once

#include "embed/types.h"
#include "tred/types.h"

enum class texture_edge
{
    clamp, repeat
};


enum class texture_render_style
{
    pixel, smooth
};


enum class transparency
{
    include, exclude
};

struct loaded_image
{
    unsigned int id;
    int width; int height;
};

loaded_image
load_image_from_embedded
(
    const embedded_binary& image_binary,
    texture_edge te,
    texture_render_style trs,
    transparency t
);

loaded_image
load_image_from_pixel_buffer
(
    void* pixels, int width, int height,
    texture_edge te,
    texture_render_style trs,
    transparency t
);

loaded_image
load_image_from_color
(
    u32 pixel,
    texture_edge te,
    texture_render_style trs,
    transparency t
);
