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


CompiledGeomVertexAttributes::CompiledGeomVertexAttributes(const CompiledVertexLayoutNoNameList& e, const VertexTypes& t)
    : elements(e)
    , debug_types(t)
{
}


std::vector<VertexType>
CompiledGeomVertexAttributes::get_base_layout() const
{
    std::vector<VertexType> r;
    for(const auto& e: elements)
    {
        r.emplace_back(e.type);
    }
    return r;
}


/** A list of things we need to extract from the Geom when compiling */
struct VertexTypeList
{
    VertexTypeList(const std::vector<VertexType>& a_base_layout)
        : base_layout(a_base_layout)
    {
    }

    void
    add(const ShaderVertexAttributes& elements)
    {
        for(const auto& e: elements)
        {
            indices.insert(e.type);
        }
    }

    std::vector<VertexType> base_layout;
    std::set<VertexType> indices;
};


CompiledVertexTypeList::CompiledVertexTypeList(const std::unordered_map<VertexType, int>& i, const ::VertexTypes& v)
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


[[nodiscard]] CompiledGeomVertexAttributes
CompiledVertexTypeList::get_mesh_layout() const
{
    CompiledGeomVertexAttributes::CompiledVertexLayoutNoNameList list;

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
    std::unordered_map<VertexType, int> indices;

    int next_index = 0;

    for(const auto type: list.base_layout)
    {
        indices.insert({type, next_index});
        next_index += ShaderAttributeSize(type);
    }

    for(const auto type: list.indices)
    {
        // already in base layout, don't add again
        if(indices.find(type) != indices.end())
        {
            continue;
        }

        indices.insert({type, next_index});
        next_index += ShaderAttributeSize(type);
    }

    return {indices, {list.indices.begin(), list.indices.end()} };
}


CompiledVertexTypeList
compile_attribute_layouts(const std::vector<VertexType>& base_layout, const std::vector<ShaderVertexAttributes>& descriptions)
{
    auto list = VertexTypeList{base_layout};

    for(const auto& d: descriptions)
    {
        list.add(d);
    }

    return compile_vertex_type_list(list);
}


CompiledVertexTypeList
compile_attribute_layouts(const std::vector<ShaderVertexAttributes>& descriptions)
{
    return compile_attribute_layouts({}, descriptions);
}

