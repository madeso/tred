#pragma once

#include <vector>
#include <map>
#include <set>
#include <string>


/** Vertex source type, position, normal etc. */
enum class vertex_type
{
    position2, position3, normal3, color4, texture2
    // change to include other textcoords and custom types that are created from scripts
};


/** A not-yet-realised binding to a shader variable like 'vec3 position' */
struct vertex_element_description
{
    vertex_type type;
    std::string name;

    vertex_element_description(vertex_type t, const std::string& n);
};

/** A realized VertexElementDescription */
struct compiled_vertex_element
{
    vertex_type type;
    std::string name;
    int index;

    compiled_vertex_element(const vertex_element_description& d, int i);
};


using vertex_layout_description = std::vector<vertex_element_description>;
using vertex_types = std::vector<vertex_type>;

/** A list of CompiledVertexElement */
struct compiled_vertex_layout
{
    using compiled_vertex_layout_list = std::vector<compiled_vertex_element>;

    compiled_vertex_layout(const compiled_vertex_layout_list& e, const vertex_types& t);

    compiled_vertex_layout_list elements;
    vertex_types types;
};

/** A list of things we need to extract from the Mesh when compiling */
struct vertex_type_list
{
    void
    add(const vertex_layout_description& elements);

    std::set<vertex_type> indices;
};

/** A mapping of the vertex type (position...) to the actual shader id */
struct compiled_vertex_type_list
{
    compiled_vertex_type_list(const std::map<vertex_type, int>& i, const vertex_types& v);

    [[nodiscard]] compiled_vertex_layout
    compile(const vertex_layout_description& elements) const;

    std::map<vertex_type, int> indices;
    vertex_types types;
};


compiled_vertex_type_list
compile(const vertex_type_list& list);


compiled_vertex_type_list
compile(const std::vector<vertex_layout_description>& descriptions);

