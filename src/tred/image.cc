#include "tred/image.h"

#include "stb_image.h"

#include "tred/opengl.h"
#include "tred/log.h"
#include "tred/cint.h"


unsigned int CreateTexture()
{
    unsigned int texture;
    glGenTextures(1, &texture);
    return texture;
}

LoadedImage
LoadImageFromPixels
(
    void* pixel_data, int width, int height,
    TextureEdge texture_edge,
    TextureRenderStyle texture_render_style,
    Transperency transperency
)
{
    const auto texture = CreateTexture();
    glBindTexture(GL_TEXTURE_2D, texture);

    const auto wrap = texture_edge == TextureEdge::Clamp ? GL_CLAMP_TO_EDGE : GL_REPEAT;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);

    const auto render_pixels = texture_render_style == TextureRenderStyle::Pixel;

    const auto min_filter = render_pixels ? GL_NEAREST : GL_LINEAR_MIPMAP_LINEAR;
    const auto mag_filter = render_pixels ? GL_NEAREST : GL_LINEAR;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);

    const auto include_transperency = transperency == Transperency::Include;

    if(pixel_data == nullptr)
    {
        LOG_ERROR("ERROR: Failed to load image.");
    }
    else
    {
        glTexImage2D
        (
            GL_TEXTURE_2D,
            0,
            include_transperency ? GL_RGBA : GL_RGB,
            width, height,
            0,
            include_transperency ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE,
            pixel_data
        );
        if(render_pixels == false)
        {
            glGenerateMipmap(GL_TEXTURE_2D);
        }
    }

    return {texture, width, height};
}

LoadedImage
LoadImage
(
    const unsigned char* image_source,
    int size,
    TextureEdge texture_edge,
    TextureRenderStyle texture_render_style,
    Transperency transperency
)
{
    int width = 0;
    int height = 0;
    int junk_channels;
    stbi_set_flip_vertically_on_load(true);
    const auto include_transperency = transperency == Transperency::Include;
    auto* pixel_data = stbi_load_from_memory
    (
        image_source, size,
        &width, &height,
        &junk_channels, include_transperency ? 4 : 3
    );

    auto loaded = LoadImageFromPixels(pixel_data, width, height, texture_edge, texture_render_style, transperency);

    if(pixel_data != nullptr)
    {
        stbi_image_free(pixel_data);
    }

    return loaded;
}

LoadedImage
LoadImageEmbeded
(
    const EmbeddedBinary& image_binary,
    TextureEdge texture_edge,
    TextureRenderStyle texture_render_style,
    Transperency transperency
)
{
    return LoadImage
    (
        reinterpret_cast<const unsigned char*>(image_binary.data),
        Cunsigned_int_to_int(image_binary.size),
        texture_edge,
        texture_render_style,
        transperency
    );
}

LoadedImage
LoadImageSingleFromSinglePixel
(
    u32 pixel,
    TextureEdge texture_edge,
    TextureRenderStyle texture_render_style,
    Transperency transperency
)
{
    return LoadImageFromPixels
    (
        &pixel, 1, 1, texture_edge, texture_render_style, transperency
    );
}
