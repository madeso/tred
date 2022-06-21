#pragma once

#include "tred/hash.string.h"
#include "tred/render/material.property.h"


namespace render
{

struct Material
{
    std::string shader;

    explicit Material(const std::string& shader_path);

    // custom properties
    // example: the "diffuse" property for all "box" meshes is "dirt.png"
    std::unordered_map<HashedString, PropertyIndex> name_to_array;
    std::vector<float> floats;
    std::vector<glm::vec3> vec3s;
    std::vector<glm::vec4> vec4s;
    std::vector<std::string> textures;

    #define ADD_OP(FUNC_NAME, TYPE)\
    Material& FUNC_NAME(const HashedString& name, const TYPE& v)
    ADD_OP(with_float, float);
    ADD_OP(with_vec3, glm::vec3);
    ADD_OP(with_vec4, glm::vec4);
    ADD_OP(with_texture, std::string);
    #undef ADD_OP
};

}
