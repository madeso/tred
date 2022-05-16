#pragma once

#include "tred/vertex_layout.h"
#include "tred/geom.h"

/** A compiled Geom*/
struct CompiledGeom
{
    unsigned int vbo;
    unsigned int vao;
    unsigned int ebo;

    int number_of_indices;

    VertexTypes debug_shader_types;

    CompiledGeom(unsigned int a_vbo, unsigned int a_vao, unsigned int a_ebo, int count, const VertexTypes& st);
    ~CompiledGeom();

    CompiledGeom(const CompiledGeom&) = delete;
    void operator=(const CompiledGeom&) = delete;

    CompiledGeom(CompiledGeom&&) = delete;
    void operator=(CompiledGeom&&) = delete;

    void
    draw() const;
};


CompiledGeom
compile_mesh(const Geom& mesh, const CompiledGeomVertexAttributes& layout);
