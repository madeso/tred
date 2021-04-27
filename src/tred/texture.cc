#include "tred/texture.h"

#include <cassert>

#include "tred/opengl.h"
#include "tred/cint.h"
#include "tred/image.h"


texture::texture(const loaded_image& i)
    : id(i.id)
    , width(i.width)
    , height(i.height)
{
}


void
texture::cleanup()
{
    // todo(Gustav): implement this
}


void
bind_texture(const uniform& uniform, const texture& texture)
{
    if(uniform.is_valid() == false) { return; }
    assert(uniform.texture >= 0);

    glActiveTexture(Cint_to_glenum(GL_TEXTURE0 + uniform.texture));
    glBindTexture(GL_TEXTURE_2D, texture.id);
}

