#pragma once

#include <vector>
#include <map>
#include <set>
#include <string>


/** Vertex source type, position, normal etc. */
enum class VertexType
{
    Position3, Normal3, Color4, Texture2

    // change to include other textcoords and custom types that are created from scripts
};


/** A not-yet-realised binding to a shader variable like 'vec3 position' */
struct VertexElementDescription
{
    VertexType type;
    std::string name;

    VertexElementDescription(VertexType t, const std::string& n);
};

/** A realized VertexElementDescription */
struct CompiledVertexElement
{
    VertexType type;
    std::string name;
    int index;

    CompiledVertexElement(const VertexElementDescription& d, int i);
};


using VertexLayoutDescription = std::vector<VertexElementDescription>;
using VertexTypes = std::vector<VertexType>;

/** A list of CompiledVertexElement */
struct CompiledVertexLayout
{
    using CompiledVertexLayoutList = std::vector<CompiledVertexElement>;

    CompiledVertexLayout(const CompiledVertexLayoutList& e, const VertexTypes& t);

    CompiledVertexLayoutList elements;
    VertexTypes types;
};

/** A list of things we need to extract from the Mesh when compiling */
struct VertexTypeList
{
    void
    Add(const VertexLayoutDescription& elements);

    std::set<VertexType> indices;
};

/** A mapping of the vertex type (position...) to the actual shader id */
struct CompiledVertexTypeList
{
    CompiledVertexTypeList(const std::map<VertexType, int>& i, const VertexTypes& v);

    CompiledVertexLayout
    Compile(const VertexLayoutDescription& elements) const;

    std::map<VertexType, int> indices;
    VertexTypes vertex_types;
};


CompiledVertexTypeList
Compile(const VertexTypeList& list);


CompiledVertexTypeList
Compile(const std::vector<VertexLayoutDescription>& descriptions);

