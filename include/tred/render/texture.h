#pragma once

#include "tred/render/uniform.h"

#include "embed/types.h"
#include "tred/types.h"

namespace render
{


enum class TextureEdge
{
    clamp, repeat
};


enum class TextureRenderStyle
{
    pixel, smooth
};


enum class Transparency
{
    include, exclude
};


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

Texture
load_image_from_embedded
(
    const embedded_binary& image_binary,
    TextureEdge te,
    TextureRenderStyle trs,
    Transparency t
);

Texture
load_image_from_color
(
    u32 pixel,
    TextureEdge te,
    TextureRenderStyle trs,
    Transparency t
);


}
