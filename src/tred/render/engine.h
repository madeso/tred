#pragma once

#include <memory>

#include "tred/render/handles.h"

struct HashedString;

namespace render
{

struct Camera;
struct CompiledMaterial;
struct EngineData;
struct MaterialDescription;
struct Mesh;
struct Vfs;
struct World;

struct AddedMesh
{
    GeomId geom;
    MaterialId material;
};

struct Engine
{
    Engine
    (
        Vfs* vfs,
        int number_of_directional_lights,
        int number_of_point_lights,
        int number_of_spot_lights
    );
    ~Engine();

    std::unique_ptr<EngineData> data;

    AddedMesh add_mesh(const Mesh& mesh);
    MaterialId add_global_shader(const MaterialDescription& path);
    MaterialId duplicate_material(MaterialId id);
    CompiledMaterial& get_material_ref(MaterialId id);
};


///////////////////////////////////////////////////////////////////////////////////////////////////
// Functions


// todo(Gustav): add util functions (that also take index) and clear prop (hashedstring+index)
#define ADD_OP(FUNC_NAME, TYPE)\
void set_##FUNC_NAME##_by_lookup\
(\
    CompiledMaterial* mat,\
    const EngineData& cache,\
    const HashedString& name,\
    const TYPE& v\
)
ADD_OP(float, float);
ADD_OP(vec3, glm::vec3);
ADD_OP(vec4, glm::vec4);
ADD_OP(texture, TextureId);
#undef ADD_OP


void render_world(Engine* engine, World& world, float aspect_ratio, const Camera& camera, std::optional<MaterialId> global_shader);
    
}
