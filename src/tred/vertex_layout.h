#pragma once

#include <vector>
#include <map>
#include <set>
#include <string>
#include <optional>


/** Vertex source type, position, normal etc. */
enum class VertexType
{
    position2, position3, normal3, color4, texture2
    // change to include other textcoords and custom types that are created from scripts
};

std::optional<VertexType> parse_vertex_type(const std::string& name);


/** A not-yet-realised binding to a shader variable like 'vec3 position' */
struct VertexElementDescription
{
    VertexType type;
    std::string name;

    VertexElementDescription(VertexType t, const std::string& n);
};



/** Describes all vertex inputs a shader requires like `[vec3 position, vec3 normal, vec2 uv]`*/
using VertexLayoutDescription = std::vector<VertexElementDescription>;



/** A realized VertexElementDescription like `vec3 position` at gl shader position 3 */
struct CompiledVertexElement
{
    VertexType type;
    std::string name;
    int index;

    CompiledVertexElement(const VertexType& t, const std::string& n, int i);
    CompiledVertexElement(const VertexElementDescription& d, int i);
};


/** A realized VertexElementDescription without name like `vec3` at gl shader position 3 */
struct CompiledVertexElementNoName
{
    VertexType type;
    int index;

    CompiledVertexElementNoName(const VertexType& t, int i);
};


using VertexTypes = std::vector<VertexType>;


/** A list of CompiledVertexElement (for shader) */
struct CompiledVertexLayout
{
    using CompiledVertexLayoutList = std::vector<CompiledVertexElement>;

    CompiledVertexLayout(const CompiledVertexLayoutList& e, const VertexTypes& t);

    CompiledVertexLayoutList elements;
    VertexTypes debug_types;
};

/** A list of CompiledVertexLayoutNoNameList (for mesh) */
struct CompiledMeshVertexLayout
{
    using CompiledVertexLayoutNoNameList = std::vector<CompiledVertexElementNoName>;

    CompiledMeshVertexLayout(const CompiledVertexLayoutNoNameList& e, const VertexTypes& t);

    CompiledVertexLayoutNoNameList elements;
    VertexTypes debug_types;
};


/** A mapping of the vertex type (position...) to the actual shader id */
struct CompiledVertexTypeList
{
    CompiledVertexTypeList(const std::map<VertexType, int>& i, const VertexTypes& v);

    [[nodiscard]] CompiledVertexLayout
    compile(const VertexLayoutDescription& elements) const;

    [[nodiscard]] CompiledMeshVertexLayout
    compile_mesh_layout() const;

    std::map<VertexType, int> indices;
    VertexTypes debug_types;
};


CompiledVertexTypeList
compile(const std::vector<VertexLayoutDescription>& descriptions);

