#include "tred/render/compiledmaterial.h"

#include "tred/assert.h"
#include "tred/log.h"
#include "tred/cint.h"

#include "tred/render/engine.data.h"
#include "tred/render/compiledmaterialshader.h"
#include "tred/render/material.h"


namespace render
{


#define ADD_OP(FUNC_NAME, MEMBER, TYPE, ENUM)\
void set_##FUNC_NAME##_by_lookup\
(\
    CompiledMaterial* mat,\
    const EngineData& data,\
    const HashedString& name,\
    const TYPE& v\
)\
{\
    const auto& cms = get_compiled_material_shader(data.cache, mat->shader);\
    const auto found = cms.name_to_array.find(name);\
    ASSERT(found != cms.name_to_array.end());\
    mat->properties.set_##FUNC_NAME(found->second, v);\
}

ADD_OP(float, floats, float, PropertyType::float_type)
ADD_OP(vec3, vec3s, glm::vec3, PropertyType::vec3_type)
ADD_OP(vec4, vec4s, glm::vec4, PropertyType::vec4_type)
ADD_OP(texture, textures, TextureId, PropertyType::texture_type)
#undef ADD_OP


void use_material
(
    const CompiledMaterial& mat,
    const Cache& cache,
    const CommonData& data,
    const LightData& light_data,
    LightStatus* ls
)
{
    const auto& shader_data = get_compiled_material_shader(cache, mat.shader);
    use_compiled_material_shader(shader_data, mat.properties, cache, data, light_data, ls);
}


CompiledMaterial
compile_material
(
    Engine* engine,
    Cache* cache,
    const Vfs& vfs,
    const Material& mat
)
{
    auto shader_id = load_compiled_material_shader(engine, vfs, cache, mat.shader);
    const auto& shader = get_compiled_material_shader(*cache, shader_id);

    auto properties = shader.default_values;

    for(const auto& nta: mat.name_to_array)
    {
        const auto found = shader.name_to_array.find(nta.first);
        if(found == shader.name_to_array.end())
        {
            LOG_ERROR("Invalid property {} in shader {}", nta.first, shader.debug_name);
            continue;
        }

        const auto& shader_index = found->second;
        const auto mat_index = nta.second;
        if(shader_index.type != mat_index.type)
        {
            LOG_ERROR("Type mismatch for {} in shader {}: {} vs {}", nta.first, shader.debug_name, shader_index.type, mat_index.type);
            continue;
        }

        switch(mat_index.type)
        {
        case PropertyType::float_type:   properties.floats  [Cint_to_sizet(shader_index.index)].value = mat.floats  [Cint_to_sizet(mat_index.index)]; break;
        case PropertyType::vec3_type:    properties.vec3s   [Cint_to_sizet(shader_index.index)].value = mat.vec3s   [Cint_to_sizet(mat_index.index)]; break;
        case PropertyType::vec4_type:    properties.vec4s   [Cint_to_sizet(shader_index.index)].value = mat.vec4s   [Cint_to_sizet(mat_index.index)]; break;
        case PropertyType::texture_type: properties.textures[Cint_to_sizet(shader_index.index)].value = load_texture(cache, vfs, mat.textures[Cint_to_sizet(mat_index.index)]); break;
        default:
            DIE("Unhandled type");
            break;
        }
    }

    return {shader_id, properties};
}

}

