#include "tred/render/engine.h"

#include "tred/render/compiled.mesh.h"
#include "tred/render/world.h"
#include "tred/render/scopedrenderer.h"


namespace render
{


Engine::Engine(Vfs* a_vfs, const LightParams& alp)
    : vfs(a_vfs)
    , lp(alp)
{
}

AddedMesh Engine::add_mesh(const Mesh& mesh)
{
    added_meshes = true;
    auto compiled = compile_mesh(this, &cache, *vfs, mesh);
    return {geoms.add(std::move(compiled.geom)), materials.add(std::move(compiled.material))};
}

MaterialId Engine::add_global_shader(const Material& path)
{
    ASSERT(added_meshes == false);
    auto material = compile_material(this, &cache, *vfs, path);
    const auto& shader = get_compiled_material_shader(cache, material.shader);
    global_layout = shader.mesh_layout.get_base_layout();
    return materials.add(std::move(material));
}

MaterialId Engine::duplicate_material(MaterialId id)
{
    CompiledMaterial copy = materials[id];
    return materials.add(std::move(copy));
}

CompiledMaterial& Engine::get_material_ref(MaterialId id)
{
    return materials[id];
}


// todo(Gustav): refactor to get lp as a argument isntead of engine?
const LightParams& get_light_params(const Engine& engine)
{
    return engine.lp;
}


CompiledVertexTypeList get_compile_attribute_layouts(Engine* engine, const ShaderVertexAttributes& layout)
{
    return compile_attribute_layouts(engine->global_layout, {layout});
}


void render_world(Engine* engine, World& world, float aspect_ratio, const Camera& camera, std::optional<MaterialId> global_shader)
{
    auto renderer = render::create_render_list_for_perspective(engine, aspect_ratio, camera, global_shader);
    world.render(&renderer);
    renderer.render_all();
}

}
