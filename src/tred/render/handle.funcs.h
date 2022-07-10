#pragma once

#include "tred/render/handles.h"


// todo(Gustav): move to a better place?

namespace render
{

struct Texture;
struct Vfs;
struct Cache;
struct Engine;
struct Vfs;
struct Cache;
struct CompiledMaterialShader;

CompiledMaterialShaderId
load_compiled_material_shader
(
    Engine* engine,
    const Vfs& vfs,
    Cache* cache,
    const std::string& path
);

const CompiledMaterialShader&
get_compiled_material_shader
(
    const Cache& cache,
    CompiledMaterialShaderId id
);

TextureId
load_texture
(
    Cache* cache,
    const Vfs& vfs,
    const std::string& path
);

const Texture&
get_texture
(
    const Cache& cache,
    TextureId id
);


}
