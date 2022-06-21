#pragma once

#include "embed/types.h"
#include "tred/types.h"
#include "tred/render/texture.types.h"
#include "tred/render/texture.h"

namespace render
{

Texture
load_image_from_embedded
(
    const embedded_binary& image_binary,
    TextureEdge te,
    TextureRenderStyle trs,
    Transparency t
);

Texture
load_image_from_color
(
    u32 pixel,
    TextureEdge te,
    TextureRenderStyle trs,
    Transparency t
);

}
