#include "sprites/cards.h"

#include "cards.png.h"

namespace cards
{
    LoadedImage load_texture()
    {
        return load_image_from_embedded
        (
            CARDS_PNG,
            TextureEdge::clamp,
            TextureRenderStyle::smooth,
            Transparency::include
        );
    }
}
