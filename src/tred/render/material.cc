#include "tred/render/material.h"

#include "tred/cint.h"

namespace render
{

Material::Material(const std::string& shader_path)
    : shader(shader_path)
{
}


#define ADD_OP(FUNC_NAME, MEMBER, TYPE, ENUM)\
Material& Material::FUNC_NAME(const HashedString& name, const TYPE& v)\
{\
    const auto index = Csizet_to_int(MEMBER.size());\
    name_to_array.insert({name, PropertyIndex{ENUM, index}});\
    MEMBER.emplace_back(v);\
    return *this;\
}
ADD_OP(with_float, floats, float, PropertyType::float_type)
ADD_OP(with_vec3, vec3s, glm::vec3, PropertyType::vec3_type)
ADD_OP(with_vec4, vec4s, glm::vec4, PropertyType::vec4_type)
ADD_OP(with_texture, textures, std::string, PropertyType::texture_type)
#undef ADD_OP


}
