#include "tred/render/cache.h"

#include "tred/log.h"
#include "tred/render/vfs.h"
#include "tred/render/light.params.h"
#include "tred/render/compiledmaterialshader.h"

namespace render
{

TextureId get_or_load_default_texture(Cache* cache)
{
    // todo(Gustav): add default texture
    return *cache->default_texture;
}

const Texture& get_texture(const Cache& cache, TextureId id)
{
    return cache.textures[id];
}

TextureId load_texture(Cache* cache, const Vfs& vfs, const std::string& path)
{
    auto texture = vfs.load_texture(path);
    if(texture)
    {
        return cache->textures.add(std::move(*texture));
    }

    // todo(Gustav): construct default texture with path? based on normal?
    LOG_WARNING("Unable to load texture {}", path);
    return get_or_load_default_texture(cache);
}


CompiledMaterialShaderId load_compiled_material_shader(Engine* engine, const Vfs& vfs, Cache* cache, const std::string& path)
{
    auto shader = load_material_shader(engine, cache, vfs, path, get_light_params(*engine));
    if(shader)
    {
        return cache->shaders.add(std::move(*shader));
    }

    // todo(Gustav): load default shader?
    LOG_ERROR("Unable to load shader {}", path);
    DIE("unable to load shader");
    return static_cast<CompiledMaterialShaderId>(0);
}

const CompiledMaterialShader& get_compiled_material_shader(const Cache& cache, CompiledMaterialShaderId id)
{
    return cache.shaders[id];
}


}
