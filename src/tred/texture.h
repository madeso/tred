#pragma once

#include "tred/uniform.h"


struct Texture
{
    unsigned int id;

    explicit Texture(unsigned int i);

    void
    Delete();
};


void
BindTexture(const Uniform& uniform, const Texture& texture);

