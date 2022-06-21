#include "tred/render/texture.h"

#include "tred/assert.h"

#include "tred/dependency_opengl.h"
#include "tred/cint.h"
#include "tred/render/texture.load.h"
#include "tred/log.h"


namespace render
{

namespace
{
    constexpr unsigned int invalid_id = 0;
    constexpr int invalid_size = -1;

    unsigned int create_texture()
    {
        unsigned int texture;
        glGenTextures(1, &texture);
        return texture;
    }
}


Texture::Texture()
    : id(invalid_id)
    , width(invalid_size)
    , height(invalid_size)
{
}


Texture::Texture
(
    void* pixel_data, int w, int h,
    TextureEdge te,
    TextureRenderStyle trs,
    Transparency t
)
    : id(create_texture())
    , width(w)
    , height(h)
{
    glBindTexture(GL_TEXTURE_2D, id);

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
        unload();
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
}


Texture::~Texture()
{
    unload();
}


Texture::Texture(Texture&& rhs)
    : id(rhs.id)
    , width(rhs.width)
    , height(rhs.height)
{
    rhs.id = invalid_id;
    rhs.width = invalid_size;
    rhs.height = invalid_size;
}


void
Texture::operator=(Texture&& rhs)
{
    unload();

    id = rhs.id;
    width = rhs.width;
    height = rhs.height;

    rhs.id = invalid_id;
    rhs.width = invalid_size;
    rhs.height = invalid_size;
}


void
Texture::unload()
{
    if(id != invalid_id)
    {
        glDeleteTextures(1, &id);
        id = invalid_id;
    }

    width = invalid_size;
    height = invalid_size;
}


void
bind_texture(const Uniform& uniform, const Texture& texture)
{
    if(uniform.is_valid() == false) { return; }
    ASSERT(uniform.texture >= 0);

    glActiveTexture(Cint_to_glenum(GL_TEXTURE0 + uniform.texture));
    glBindTexture(GL_TEXTURE_2D, texture.id);
}

}
