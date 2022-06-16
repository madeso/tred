#include "tred/render/material.compiledprops.h"

#include "tred/render/shader.h"
#include "tred/cint.h"

#include "tred/render/material.property.h"
#include "tred/render/texture.h"

namespace render
{


#define ADD_OP(FUNC_NAME, MEMBER, TYPE, ENUM)\
void CompiledProperties::FUNC_NAME(PropertyIndex where, const TYPE& v)\
{\
    assert(where.type == ENUM);\
    MEMBER[Cint_to_sizet(where.index)].value = v;\
}
ADD_OP(set_float, floats, float, PropertyType::float_type)
ADD_OP(set_vec3, vec3s, glm::vec3, PropertyType::vec3_type)
ADD_OP(set_vec4, vec4s, glm::vec4, PropertyType::vec4_type)
ADD_OP(set_texture, textures, TextureId, PropertyType::texture_type)
#undef ADD_OP


void CompiledProperties::set_shader(const ShaderProgram& shader, const Cache& cache, const std::vector<Uniform>& uniforms) const
{
    for(const auto& d: floats)
    {
        shader.set_float(uniforms[Cint_to_sizet(d.index)], d.value);
    }
    for(const auto& d: vec3s)
    {
        shader.set_vec3(uniforms[Cint_to_sizet(d.index)], d.value);
    }
    for(const auto& d: vec4s)
    {
        shader.set_vec4(uniforms[Cint_to_sizet(d.index)], d.value);
    }
    for(const auto& d: textures)
    {
        const auto& uniform = uniforms[Cint_to_sizet(d.index)];
        bind_texture(uniform, get_texture(cache, d.value));
        shader.set_texture(uniform);
    }
}


}
