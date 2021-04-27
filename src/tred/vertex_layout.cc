#include "tred/vertex_layout.h"

#include <cassert>


vertex_element_description::vertex_element_description(vertex_type t, const std::string& n)
    : type(t)
    , name(n)
{
}


compiled_vertex_element::compiled_vertex_element(const vertex_element_description& d, int i)
    : type(d.type)
    , name(d.name)
    , index(i)
{
}


compiled_vertex_layout::compiled_vertex_layout(const compiled_vertex_layout_list& e, const vertex_types& t)
    : elements(e)
    , types(t)
{
}


void
vertex_type_list::add(const vertex_layout_description& elements)
{
    for(const auto& e: elements)
    {
        indices.insert(e.type);
    }
}


compiled_vertex_type_list::compiled_vertex_type_list(const std::map<vertex_type, int>& i, const ::vertex_types& v)
    : indices(i)
    , types(v)
{
}

compiled_vertex_layout
compiled_vertex_type_list::compile(const vertex_layout_description& elements) const
{
    compiled_vertex_layout::compiled_vertex_layout_list list;

    for(const auto& e: elements)
    {
        const auto found = indices.find(e.type);
        assert(found != indices.end() && "layout wasn't added to the compilation list");

        list.push_back({e, found->second});
    }

    return {list, types};
}


int
ShaderAttributeSize(const vertex_type&)
{
    return 1;
}


compiled_vertex_type_list
compile(const vertex_type_list& list)
{
    std::map<vertex_type, int> indices;

    int next_index = 0;
    for(const auto type: list.indices)
    {
        indices[type] = next_index;
        next_index += ShaderAttributeSize(type);
    }

    return {indices, {list.indices.begin(), list.indices.end()} };
}


compiled_vertex_type_list
compile(const std::vector<vertex_layout_description>& descriptions)
{
    vertex_type_list list;

    for(const auto& d: descriptions)
    {
        list.add(d);
    }

    return compile(list);
}

