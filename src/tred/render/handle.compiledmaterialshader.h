#pragma once

#include "tred/types.h"


namespace render
{

struct Engine;
struct Vfs;
struct Cache;
struct CompiledMaterialShader;

enum class CompiledMaterialShaderId : u64 {};

CompiledMaterialShaderId load_compiled_material_shader(Engine* engine, const Vfs& vfs, Cache* cache, const std::string& path);
const CompiledMaterialShader& get_compiled_material_shader(const Cache& cache, CompiledMaterialShaderId id);

}

