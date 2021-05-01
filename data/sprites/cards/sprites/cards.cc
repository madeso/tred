#include "sprites/cards.h"

#include "cards.png.h"

namespace cards
{
    loaded_image load_texture()
    {
        return load_image_from_embedded
        (
            CARDS_PNG,
            texture_edge::clamp,
            texture_render_style::smooth,
            transparency::include
        );
    }
}
