#include "tred/render/compiled.mesh.h"

#include "tred/render/mesh.h"
#include "tred/render/compiledmaterialshader.h"

namespace render
{


CompiledMesh compile_mesh(Engine* engine, Cache* cache, const Vfs& vfs, const Mesh& mesh)
{
    auto material = compile_material(engine, cache, vfs, mesh.material);
    const auto& shader = get_compiled_material_shader(*cache, material.shader);
    auto geom = compile_geom(mesh.geom, shader.mesh_layout);

    return {std::move(geom), material};
}


}
