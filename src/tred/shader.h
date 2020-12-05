#pragma once

#include <string_view>

#include "glm/glm.hpp"

#include "tred/vertex_layout.h"
#include "tred/uniform.h"


struct Shader
{
    Shader
    (
        std::string_view vertex_source,
        std::string_view fragment_source,
        const CompiledVertexLayout& layout
    );

    void
    Use() const;

    void
    Delete();

    Uniform
    GetUniform(const std::string& name) const;

    // shader neeeds to be bound
    void
    SetFloat(const Uniform& uniform, float value) const;

    void
    SetVec3(const Uniform& uniform, float x, float y, float z);

    void
    SetVec3(const Uniform& uniform, const glm::vec3& v);

    void
    SetVec4(const Uniform& uniform, float x, float y, float z, float w);

    void
    SetVec4(const Uniform& uniform, const glm::vec4& v);

    void
    SetTexture(const Uniform& uniform);

    void
    SetMat(const Uniform& uniform, const glm::mat4& mat);

    void
    SetMat(const Uniform& uniform, const glm::mat3& mat);


    unsigned int shader_program;
    VertexTypes vertex_types;
};


void
SetupTextures(Shader* shader, std::vector<Uniform*> uniform_list);

