#pragma once

#include "tred/render/material.compiledprops.h"
#include "tred/render/handle.compiledmaterialshader.h"
#include "tred/render/handle.texture.h"

struct HashedString;


namespace render
{

struct Cache;
struct CommonData;
struct LightData;
struct LightStatus;
struct Engine;
struct Vfs;
struct Material;

struct CompiledMaterial
{
    CompiledMaterialShaderId shader;
    CompiledProperties properties;

    // todo(Gustav): add util functions (that also take index) and clear prop (hashedstring+index)

    #define ADD_OP(FUNC_NAME, TYPE)\
    void set_##FUNC_NAME##_by_lookup(const Cache& cache, const HashedString& name, const TYPE& v)
    ADD_OP(float, float);
    ADD_OP(vec3, glm::vec3);
    ADD_OP(vec4, glm::vec4);
    ADD_OP(texture, TextureId);
    #undef ADD_OP

    void use(const Cache& cache, const CommonData& data, const LightData& light_data, LightStatus* ls) const;
};


CompiledMaterial compile_material(Engine* engine, Cache* cache, const Vfs& vfs, const Material& mat);

}
