#pragma once

#include <string_view>

#include "glm/glm.hpp"

#include "tred/vertex_layout.h"
#include "tred/uniform.h"


struct shader
{
    shader
    (
        std::string_view vertex_source,
        std::string_view fragment_source,
        const compiled_vertex_layout& layout
    );

    void
    use() const;

    void
    cleanup();

    uniform
    get_uniform(const std::string& name) const;

    // shader needs to be bound
    void
    set_float(const uniform& uniform, float value) const;

    void
    set_vec3(const uniform& uniform, float x, float y, float z);

    void
    set_vec3(const uniform& uniform, const glm::vec3& v);

    void
    set_vec4(const uniform& uniform, float x, float y, float z, float w);

    void
    set_vec4(const uniform& uniform, const glm::vec4& v);

    void
    set_texture(const uniform& uniform);

    void
    set_mat(const uniform& uniform, const glm::mat4& mat);

    void
    set_mat(const uniform& uniform, const glm::mat3& mat);


    unsigned int shader_program;
    vertex_types vertex_types;
};


void
setup_textures(shader* shader, std::vector<uniform*> uniform_list);

