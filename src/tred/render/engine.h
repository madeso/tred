#pragma once

#include "tred/geom.compiled.h"

#include "tred/render/handle.mesh.h"
#include "tred/render/compiledmaterial.h"
#include "tred/render/light.params.h"
#include "tred/render/cache.h"
#include "tred/render/renderlist.h"
#include "tred/handle.h"

namespace render
{


struct Vfs;
struct Mesh;
struct Material;


struct AddedMesh
{
    GeomId geom;
    MaterialId material;
};

struct Engine
{
    Engine(Vfs* a_vfs, const LightParams& alp);

    Vfs* vfs;
    LightParams lp;
    Cache cache;
    HandleVector64<CompiledGeom, GeomId> geoms;
    HandleVector64<CompiledMaterial, MaterialId> materials;
    std::vector<VertexType> global_layout;
    bool added_meshes = false;
    RenderList render;


    AddedMesh add_mesh(const Mesh& mesh);
    MaterialId add_global_shader(const Material& path);
    MaterialId duplicate_material(MaterialId id);
    CompiledMaterial& get_material_ref(MaterialId id);
    
};
    
}
