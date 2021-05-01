#include "sprites/onebit.h"

#include "tred/image.h"

#include "onebit.png.h"

namespace onebit
{
    loaded_image load_texture()
    {
        return load_image_from_embedded
        (
            ONEBIT_PNG,
            texture_edge::clamp,
            texture_render_style::pixel,
            transparency::include
        );
    }
}
