#include "sprites/onebit.h"

#include "tred/texture.load.h"

#include "onebit.png.h"

namespace onebit
{
    Texture load_texture()
    {
        return load_image_from_embedded
        (
            ONEBIT_PNG,
            TextureEdge::clamp,
            TextureRenderStyle::pixel,
            Transparency::include
        );
    }
}
