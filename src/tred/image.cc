#include "tred/image.h"

#include "stb_image.h"

#include "tred/dependency_opengl.h"
#include "tred/log.h"
#include "tred/cint.h"


unsigned int create_texture()
{
    unsigned int texture;
    glGenTextures(1, &texture);
    return texture;
}


LoadedImage
load_image_from_pixel_buffer
(
    void* pixel_data, int width, int height,
    TextureEdge te,
    TextureRenderStyle trs,
    Transparency t
)
{
    const auto texture = create_texture();
    glBindTexture(GL_TEXTURE_2D, texture);

    const auto wrap = te == TextureEdge::clamp ? GL_CLAMP_TO_EDGE : GL_REPEAT;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);

    const auto render_pixels = trs == TextureRenderStyle::pixel;

    const auto min_filter = render_pixels ? GL_NEAREST : GL_LINEAR_MIPMAP_LINEAR;
    const auto mag_filter = render_pixels ? GL_NEAREST : GL_LINEAR;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);

    const auto include_transparency = t == Transparency::include;

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
            include_transparency ? GL_RGBA : GL_RGB,
            width, height,
            0,
            include_transparency ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE,
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
    TextureEdge te,
    TextureRenderStyle trs,
    Transparency t
)
{
    int width = 0;
    int height = 0;
    int junk_channels;
    stbi_set_flip_vertically_on_load(true);
    const auto include_transparency = t == Transparency::include;
    auto* pixel_data = stbi_load_from_memory
    (
        image_source, size,
        &width, &height,
        &junk_channels, include_transparency ? 4 : 3
    );

    const auto loaded = load_image_from_pixel_buffer(pixel_data, width, height, te, trs, t);

    if(pixel_data != nullptr)
    {
        stbi_image_free(pixel_data);
    }

    return loaded;
}


LoadedImage
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


LoadedImage
load_image_from_color
(
    u32 pixel,
    TextureEdge te,
    TextureRenderStyle trs,
    Transparency t
)
{
    return load_image_from_pixel_buffer
    (
        &pixel, 1, 1, te, trs, t
    );
}
