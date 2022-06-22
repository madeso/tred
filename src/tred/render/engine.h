#pragma once

#include <memory>

#include "tred/render/handle.mesh.h"


namespace render
{

struct Camera;
struct CompiledMaterial;
struct EngineData;
struct Material;
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
    MaterialId add_global_shader(const Material& path);
    MaterialId duplicate_material(MaterialId id);
    CompiledMaterial& get_material_ref(MaterialId id);
};


void render_world(Engine* engine, World& world, float aspect_ratio, const Camera& camera, std::optional<MaterialId> global_shader);
    
}
