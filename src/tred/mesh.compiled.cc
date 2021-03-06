#include "tred/mesh.compiled.h"

#include <functional>
#include <numeric>

#include "tred/opengl.h"
#include "tred/cint.h"
#include "tred/shader.util.h"


unsigned int
CreateBuffer()
{
    unsigned int buffer;
    glGenBuffers(1, &buffer);
    return buffer;
};


unsigned int
CreateVertexArray()
{
    unsigned int vao;
    glGenVertexArrays(1, &vao);
    return vao;
};


CompiledMesh::CompiledMesh(unsigned int a_vbo, unsigned int a_vao, unsigned int a_ebo, int count, const vertex_types& st)
        : vbo(a_vbo)
        , vao(a_vao)
        , ebo(a_ebo)
        , number_of_indices(count)
        , debug_shader_types(st)
{
}


void
CompiledMesh::Draw() const
{
    assert(is_bound_for_shader(debug_shader_types));
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, number_of_indices, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}


void
CompiledMesh::Delete() const
{
    glBindVertexArray(0);
    glDeleteVertexArrays(1, &vao);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &ebo);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &vbo);
}


struct BufferData
{
    using PerVertex = std::function<void (std::vector<float>*, const vertex&)>;

    int count;
    PerVertex per_vertex;
    int start = 0;

    BufferData(int c, PerVertex pv)
        : count(c)
        , per_vertex(pv)
    {
    }
};


CompiledMesh
Compile(const mesh& mesh, const compiled_vertex_layout& layout)
{
    using VertexVector = std::vector<float>;

    auto data = std::vector<BufferData>{};
    data.reserve(layout.elements.size());

    for(const auto& element: layout.elements)
    {
        switch(element.type)
        {
        case vertex_type::position3:
            data.emplace_back(3, [](VertexVector* vertices, const vertex& vertex)
            {
                vertices->push_back(vertex.position.x);
                vertices->push_back(vertex.position.y);
                vertices->push_back(vertex.position.z);
            });
            break;
        case vertex_type::normal3:
            data.emplace_back(3, [](VertexVector* vertices, const vertex& vertex)
            {
                vertices->push_back(vertex.normal.x);
                vertices->push_back(vertex.normal.y);
                vertices->push_back(vertex.normal.z);
            });
            break;
        case vertex_type::color4:
            data.emplace_back(4, [](VertexVector* vertices, const vertex& vertex)
            {
                vertices->push_back(vertex.color.x);
                vertices->push_back(vertex.color.y);
                vertices->push_back(vertex.color.z);
                vertices->push_back(vertex.color.w);
            });
            break;
        case vertex_type::texture2:
            data.emplace_back(2, [](VertexVector* vertices, const vertex& vertex)
            {
                vertices->push_back(vertex.texture.x);
                vertices->push_back(vertex.texture.y);
            });
            break;
        default:
            assert(false && "unhandled buffer type");
            break;
        }
    }

    const auto floats_per_vertex = Cint_to_sizet
    (
        std::accumulate
        (
            data.begin(), data.end(),
            0, [](auto s, const auto& d)
            {
                return s + d.count;
            }
        )
    );
    auto vertices = VertexVector{};
    vertices.reserve(mesh.vertices.size() * floats_per_vertex);
    for(const auto& vertex: mesh.vertices)
    for(const auto& d: data)
    {
        d.per_vertex(&vertices, vertex);
    }

    const auto vbo = CreateBuffer();
    const auto vao = CreateVertexArray();
    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData
    (
        GL_ARRAY_BUFFER,
        Csizet_to_glsizeiptr(vertices.size() * sizeof(float)),
        &vertices[0],
        GL_STATIC_DRAW
    );

    const auto stride = floats_per_vertex * sizeof(float);
    int location = 0;
    std::size_t offset = 0;
    for(const auto& d: data)
    {
        const auto normalize = false;
        glVertexAttribPointer
        (
            Cint_to_gluint(location),
            d.count,
            GL_FLOAT,
            normalize ? GL_TRUE : GL_FALSE,
            Csizet_to_glsizei(stride),
            reinterpret_cast<void*>(offset)
        );
        glEnableVertexAttribArray(Cint_to_gluint(location));

        location += 1;
        offset += Cint_to_sizet(d.count) * sizeof(float);
    }

    // class: use IndexBuffer as it reflects the usage better than element buffer object?
    const auto ebo = CreateBuffer();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData
    (
        GL_ELEMENT_ARRAY_BUFFER,
        Csizet_to_glsizeiptr(mesh.triangles.size() * sizeof(unsigned int)),
        &mesh.triangles[0],
        GL_STATIC_DRAW
    );

    return CompiledMesh{vbo, vao, ebo, Csizet_to_int(mesh.triangles.size()), layout.types};
}

