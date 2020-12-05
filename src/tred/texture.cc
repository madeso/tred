#include "tred/texture.h"

#include <cassert>

#include "tred/opengl.h"
#include "tred/cint.h"


Texture::Texture(unsigned int i)
    : id(i)
{
}


void
Texture::Delete()
{
    // todo(Gustav): implement this
}


void
BindTexture(const Uniform& uniform, const Texture& texture)
{
    if(uniform.IsValid() == false) { return; }
    assert(uniform.texture >= 0);

    glActiveTexture(Cint_to_glenum(GL_TEXTURE0 + uniform.texture));
    glBindTexture(GL_TEXTURE_2D, texture.id);
}

