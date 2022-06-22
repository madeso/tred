#include "tred/render/engine.h"

#include "tred/render/engine.data.h"
#include "tred/render/compiled.mesh.h"
#include "tred/render/world.h"
#include "tred/render/scopedrenderer.h"


namespace render
{


EngineData::EngineData(Vfs* a_vfs, const LightParams& alp)
    : vfs(a_vfs)
    , lp(alp)
{
}

Engine::Engine
(
    Vfs* vfs,
    int number_of_directional_lights,
    int number_of_point_lights,
    int number_of_spot_lights
)
    : data
    (
        std::make_unique<EngineData>
        (
            vfs,
            LightParams
                {
                    number_of_directional_lights,
                    number_of_point_lights,
                    number_of_spot_lights
                }
        )
    )
{
}

Engine::~Engine() = default;

AddedMesh Engine::add_mesh(const Mesh& mesh)
{
    data->added_meshes = true;
    auto compiled = compile_mesh(this, &data->cache, *data->vfs, mesh);
    return {data->geoms.add(std::move(compiled.geom)), data->materials.add(std::move(compiled.material))};
}

MaterialId Engine::add_global_shader(const Material& path)
{
    ASSERT(data->added_meshes == false);
    auto material = compile_material(this, &data->cache, *data->vfs, path);
    const auto& shader = get_compiled_material_shader(data->cache, material.shader);
    data->global_layout = shader.mesh_layout.get_base_layout();
    return data->materials.add(std::move(material));
}

MaterialId Engine::duplicate_material(MaterialId id)
{
    CompiledMaterial copy = data->materials[id];
    return data->materials.add(std::move(copy));
}

CompiledMaterial& Engine::get_material_ref(MaterialId id)
{
    return data->materials[id];
}


// todo(Gustav): refactor to get lp as a argument isntead of engine?
const LightParams& get_light_params(const Engine& engine)
{
    return engine.data->lp;
}


CompiledVertexTypeList get_compile_attribute_layouts(Engine* engine, const ShaderVertexAttributes& layout)
{
    return compile_attribute_layouts(engine->data->global_layout, {layout});
}


void render_world(Engine* engine, World& world, float aspect_ratio, const Camera& camera, std::optional<MaterialId> global_shader)
{
    auto renderer = render::create_render_list_for_perspective(engine->data.get(), aspect_ratio, camera, global_shader);
    world.render(&renderer);
    renderer.render_all();
}

}
