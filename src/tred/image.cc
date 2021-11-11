#include "tred/image.h"

#include "stb_image.h"
#include "tred/log.h"
#include "tred/cint.h"


Texture
LoadImage
(
    const unsigned char* image_source,
    int size,
    TextureEdge te,
    TextureRenderStyle trs,
    Transparency t
)
{
    const auto include_transparency = t == Transparency::include;

    int width = 0;
    int height = 0;
    int junk_channels = 0;
    
    stbi_set_flip_vertically_on_load(true);
    auto* pixel_data = stbi_load_from_memory
    (
        image_source, size,
        &width, &height,
        &junk_channels, include_transparency ? 4 : 3
    );

    if(pixel_data == nullptr)
    {
        LOG_ERROR("ERROR: Failed to load image from image source");
        return {};
    }

    auto loaded = Texture{pixel_data, width, height, te, trs, t};

    if(pixel_data != nullptr)
    {
        stbi_image_free(pixel_data);
    }

    return loaded;
}


Texture
load_image_from_embedded
(
    const embedded_binary& image_binary,
    TextureEdge te,
    TextureRenderStyle trs,
    Transparency t
)
{
    return LoadImage
    (
        reinterpret_cast<const unsigned char*>(image_binary.data),
        Cunsigned_int_to_int(image_binary.size),
        te,
        trs,
        t
    );
}


Texture
load_image_from_color
(
    u32 pixel,
    TextureEdge te,
    TextureRenderStyle trs,
    Transparency t
)
{
    return
    {
        &pixel, 1, 1, te, trs, t
    };
}
