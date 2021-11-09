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

    VertexTypes debug_shader_types;

    CompiledMesh(unsigned int a_vbo, unsigned int a_vao, unsigned int a_ebo, int count, const VertexTypes& st);
    ~CompiledMesh();

    CompiledMesh(const CompiledMesh&) = delete;
    void operator=(const CompiledMesh&) = delete;

    CompiledMesh(CompiledMesh&&) = delete;
    void operator=(CompiledMesh&&) = delete;

    void
    draw() const;
};


CompiledMesh
compile(const Mesh& mesh, const CompiledVertexLayout& layout);
