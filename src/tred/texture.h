#pragma once

#include "tred/uniform.h"

struct LoadedImage;

struct Texture
{
    unsigned int id;
    int width;
    int height;

    explicit Texture(const LoadedImage& i);

    void
    cleanup();
};


void
bind_texture(const Uniform& uniform, const Texture& texture);

