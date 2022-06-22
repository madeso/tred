#pragma once

#include "tred/render/compiled.geom.h"
#include "tred/render/compiledmaterial.h"
#include "tred/render/light.params.h"
#include "tred/render/cache.h"
#include "tred/render/renderlist.h"
#include "tred/handle.h"


namespace render
{
    struct EngineData
    {
        Vfs* vfs;
        LightParams lp;
        Cache cache;
        HandleVector64<CompiledGeom, GeomId> geoms;
        HandleVector64<CompiledMaterial, MaterialId> materials;
        std::vector<VertexType> global_layout;
        bool added_meshes = false;
        RenderList render;

        EngineData(Vfs* a_vfs, const LightParams& alp);
    };
}
