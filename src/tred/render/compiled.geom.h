#pragma once

#include <unordered_set>

#include "tred/render/vertex_layout.h"
#include "tred/render/geom.h"

namespace render
{

/** A compiled Geom*/
struct CompiledGeom
{
    unsigned int vbo;
    unsigned int vao;
    unsigned int ebo;

    int number_of_indices;

    std::unordered_set<VertexType> debug_shader_types;

    CompiledGeom(unsigned int a_vbo, unsigned int a_vao, unsigned int a_ebo, int count, const VertexTypes& st);
    CompiledGeom(CompiledGeom&& rhs);
    void operator=(CompiledGeom&&);
    ~CompiledGeom();

    void clear();

    CompiledGeom(const CompiledGeom&) = delete;
    void operator=(const CompiledGeom&) = delete;

    void
    draw() const;
};


CompiledGeom
compile_geom(const Geom& mesh, const CompiledGeomVertexAttributes& layout);

}
