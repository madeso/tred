#include "tred/texture.h"

#include <cassert>

#include "tred/dependency_opengl.h"
#include "tred/cint.h"
#include "tred/image.h"


Texture::Texture(const LoadedImage& i)
    : id(i.id)
    , width(i.width)
    , height(i.height)
{
}


Texture::~Texture()
{
    glDeleteTextures(1, &id);
    id = 0;
}


void
bind_texture(const Uniform& uniform, const Texture& texture)
{
    if(uniform.is_valid() == false) { return; }
    assert(uniform.texture >= 0);

    glActiveTexture(Cint_to_glenum(GL_TEXTURE0 + uniform.texture));
    glBindTexture(GL_TEXTURE_2D, texture.id);
}

