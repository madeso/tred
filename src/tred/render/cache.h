#pragma once

#include "tred/handle.h"
#include "tred/texture.h"
#include "tred/render/compiledmaterialshader.h"

#include "tred/render/handle.texture.h"
#include "tred/render/handle.compiledmaterialshader.h"

namespace render
{

struct Cache
{
    TextureId get_or_load_default_texture();

    HandleVector64<Texture, TextureId> textures;
    HandleVector64<CompiledMaterialShader, CompiledMaterialShaderId> shaders;

    std::optional<TextureId> default_texture;
};

}
