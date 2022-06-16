#pragma once

#include "tred/render/uniform.h"
#include "tred/render/texture.types.h"

struct Texture
{
    unsigned int id;
    int width;
    int height;

    Texture(); // invalid texture
    
    // "internal"
    Texture
    (
        void* pixel_data, int w, int h,
        TextureEdge te,
        TextureRenderStyle trs,
        Transparency t
    );

    ~Texture();


    Texture(const Texture&) = delete;
    void operator=(const Texture&) = delete;
    
    Texture(Texture&&);
    void operator=(Texture&&);

    // clears the loaded texture to a invalid texture
    void unload();
};


// set the texture for the specified uniform
void
bind_texture(const Uniform& uniform, const Texture& texture);

