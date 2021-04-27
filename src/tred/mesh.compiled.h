#pragma once

#include "tred/vertex_layout.h"
#include "tred/mesh.h"

/** A compiled Mesh*/
struct CompiledMesh
{
    unsigned int vbo;
    unsigned int vao;
    unsigned int ebo;

    int number_of_indices;

    vertex_types debug_shader_types;

    CompiledMesh(unsigned int a_vbo, unsigned int a_vao, unsigned int a_ebo, int count, const vertex_types& st);

    void
    Draw() const;

    void
    Delete() const;
};


CompiledMesh
Compile(const mesh& mesh, const compiled_vertex_layout& layout);
