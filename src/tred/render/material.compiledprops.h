#pragma once

#include "tred/render/handle.texture.h"

struct ShaderProgram;
struct Uniform;

namespace render
{

struct Cache;
struct PropertyIndex;


template<typename T>
struct UniformIndexAnd
{
    int index;
    T value;
};


struct CompiledProperties
{
    std::vector<UniformIndexAnd<float>> floats;
    std::vector<UniformIndexAnd<glm::vec3>> vec3s;
    std::vector<UniformIndexAnd<glm::vec4>> vec4s;
    std::vector<UniformIndexAnd<TextureId>> textures;

    #define ADD_OP(FUNC_NAME, TYPE)\
    void FUNC_NAME(PropertyIndex where, const TYPE& v)

    ADD_OP(set_float, float);
    ADD_OP(set_vec3, glm::vec3);
    ADD_OP(set_vec4, glm::vec4);
    ADD_OP(set_texture, TextureId);
    #undef ADD_OP

    void set_shader(const ShaderProgram& shader, const Cache& cache, const std::vector<Uniform>& uniforms) const;
};


}
