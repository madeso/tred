#pragma once

#include "tred/geom.compiled.h"
#include "tred/render/compiledmaterial.h"


namespace render
{

struct Engine;
struct Cache;
struct Vfs;
struct Mesh;

struct CompiledMesh
{
    CompiledGeom geom;
    CompiledMaterial material;
};

CompiledMesh compile_mesh(Engine* engine, Cache* cache, const Vfs& vfs, const Mesh& mesh);

}
