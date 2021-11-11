#include "sprites/cards.h"

#include "tred/image.h"

#include "cards.png.h"

namespace cards
{
    Texture load_texture()
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
