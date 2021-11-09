#pragma once

#include "tred/uniform.h"

struct LoadedImage;

struct Texture
{
    unsigned int id;
    int width;
    int height;

    explicit Texture(const LoadedImage& i);
    ~Texture();

    Texture(const Texture&) = delete;
    void operator=(const Texture&) = delete;
    
    Texture(Texture&&) = delete;
    void operator=(Texture&&) = delete;
};


void
bind_texture(const Uniform& uniform, const Texture& texture);

