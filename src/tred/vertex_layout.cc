#include "tred/vertex_layout.h"

#include <cassert>


VertexElementDescription::VertexElementDescription(VertexType t, const std::string& n)
    : type(t)
    , name(n)
{
}


CompiledVertexElement::CompiledVertexElement(const VertexType& t, const std::string& n, int i)
    : type(t)
    , name(n)
    , index(i)
{
}


CompiledVertexElement::CompiledVertexElement(const VertexElementDescription& d, int i)
    : type(d.type)
    , name(d.name)
    , index(i)
{
}


CompiledVertexElementNoName::CompiledVertexElementNoName(const VertexType& t, int i)
    : type(t)
    , index(i)
{
}


CompiledVertexLayout::CompiledVertexLayout(const CompiledVertexLayoutList& e, const VertexTypes& t)
    : elements(e)
    , types(t)
{
}


CompiledMeshVertexLayout::CompiledMeshVertexLayout(const CompiledVertexLayoutNoNameList& e, const VertexTypes& t)
    : elements(e)
    , types(t)
{
}


void
VertexTypeList::add(const VertexLayoutDescription& elements)
{
    for(const auto& e: elements)
    {
        indices.insert(e.type);
    }
}


CompiledVertexTypeList::CompiledVertexTypeList(const std::map<VertexType, int>& i, const ::VertexTypes& v)
    : indices(i)
    , types(v)
{
}

CompiledVertexLayout
CompiledVertexTypeList::compile(const VertexLayoutDescription& elements) const
{
    CompiledVertexLayout::CompiledVertexLayoutList list;

    for(const auto& e: elements)
    {
        const auto found = indices.find(e.type);
        assert(found != indices.end() && "layout wasn't added to the compilation list");

        list.push_back({e, found->second});
    }

    return {list, types};
}


[[nodiscard]] CompiledMeshVertexLayout
CompiledVertexTypeList::compile_mesh_layout() const
{
    CompiledMeshVertexLayout::CompiledVertexLayoutNoNameList list;

    for(const auto& e: indices)
    {
        list.push_back({e.first, e.second});
    }

    return {list, types};
}


int
ShaderAttributeSize(const VertexType&)
{
    return 1;
}


CompiledVertexTypeList
compile(const VertexTypeList& list)
{
    std::map<VertexType, int> indices;

    int next_index = 0;
    for(const auto type: list.indices)
    {
        indices[type] = next_index;
        next_index += ShaderAttributeSize(type);
    }

    return {indices, {list.indices.begin(), list.indices.end()} };
}


CompiledVertexTypeList
compile(const std::vector<VertexLayoutDescription>& descriptions)
{
    VertexTypeList list;

    for(const auto& d: descriptions)
    {
        list.add(d);
    }

    return compile(list);
}

