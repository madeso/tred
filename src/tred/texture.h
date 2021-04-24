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
    Delete();
};


void
BindTexture(const Uniform& uniform, const Texture& texture);

