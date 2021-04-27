#pragma once

#include "tred/uniform.h"

struct loaded_image;

struct texture
{
    unsigned int id;
    int width;
    int height;

    explicit texture(const loaded_image& i);

    void
    cleanup();
};


void
bind_texture(const uniform& uniform, const texture& texture);

