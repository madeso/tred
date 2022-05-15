#include "tred/vertex_layout.h"

#include "tred/assert.h"


std::optional<VertexType> parse_vertex_type(const std::string& name)
{
    #define NAME(x) if(name == #x) { return VertexType::x; }

    NAME(position2)
    else NAME(position3)
    else NAME(normal3)
    else NAME(color4)
    else NAME(texture2)
    else return {};
    #undef NAME
}


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


CompiledShaderVertexAttributes::CompiledShaderVertexAttributes(const CompiledVertexLayoutList& e, const VertexTypes& t)
    : elements(e)
    , debug_types(t)
{
}


CompiledMeshVertexAttributes::CompiledMeshVertexAttributes(const CompiledVertexLayoutNoNameList& e, const VertexTypes& t)
    : elements(e)
    , debug_types(t)
{
}


/** A list of things we need to extract from the Mesh when compiling */
struct VertexTypeList
{
    void
    add(const ShaderVertexAttributes& elements)
    {
        for(const auto& e: elements)
        {
            indices.insert(e.type);
        }
    }

    std::set<VertexType> indices;
};


CompiledVertexTypeList::CompiledVertexTypeList(const std::map<VertexType, int>& i, const ::VertexTypes& v)
    : indices(i)
    , debug_types(v)
{
}

CompiledShaderVertexAttributes
CompiledVertexTypeList::compile_shader_layout(const ShaderVertexAttributes& elements) const
{
    CompiledShaderVertexAttributes::CompiledVertexLayoutList list;

    for(const auto& e: elements)
    {
        const auto found = indices.find(e.type);
        ASSERT(found != indices.end() && "layout wasn't added to the compilation list");

        list.push_back({e, found->second});
    }

    return {list, debug_types};
}


[[nodiscard]] CompiledMeshVertexAttributes
CompiledVertexTypeList::get_mesh_layout() const
{
    CompiledMeshVertexAttributes::CompiledVertexLayoutNoNameList list;

    for(const auto& e: indices)
    {
        list.push_back({e.first, e.second});
    }

    return {list, debug_types};
}


int
ShaderAttributeSize(const VertexType&)
{
    return 1;
}


CompiledVertexTypeList
compile_vertex_type_list(const VertexTypeList& list)
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
compile_attribute_layouts(const std::vector<ShaderVertexAttributes>& descriptions)
{
    VertexTypeList list;

    for(const auto& d: descriptions)
    {
        list.add(d);
    }

    return compile_vertex_type_list(list);
}

