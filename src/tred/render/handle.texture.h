#pragma once

#include "tred/types.h"


namespace render
{
    
    struct Texture;
    struct Vfs;
    struct Cache;

    enum class TextureId : u64 {};


    TextureId load_texture(Cache* cache, const Vfs& vfs, const std::string& path);
    const Texture& get_texture(const Cache& cache, TextureId id);
}