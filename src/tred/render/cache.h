#pragma once

#include "tred/handle.h"
#include "tred/render/texture.h"
#include "tred/render/compiledmaterialshader.h"

#include "tred/render/handle.texture.h"
#include "tred/render/handle.compiledmaterialshader.h"

namespace render
{

struct Cache
{
    HandleVector64<Texture, TextureId> textures;
    HandleVector64<CompiledMaterialShader, CompiledMaterialShaderId> shaders;

    std::optional<TextureId> default_texture;
};

}
