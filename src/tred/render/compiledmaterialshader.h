#pragma once

#include "tred/render/uniform.h"
#include "tred/render/vertex_layout.h"
#include "tred/hash.string.h"
#include "tred/render/shader.h"

#include "tred/render/material.property.h"
#include "tred/render/light.uniforms.h"
#include "tred/render/material.compiledprops.h"


namespace render
{


struct ShaderProgram;
struct Engine;

struct CommonData
{
    glm::vec3 camera_position;
    glm::mat4 pv; // projection * view
    glm::mat4 model;
};

struct CommonUniforms
{
    CommonUniforms(const ShaderProgram& shader);

    Uniform transform;
    Uniform model_transform;
    Uniform normal_matrix;
    Uniform view_position;

    void set_shader(const ShaderProgram& shader, const CommonData& data) const;
};

struct CompiledMaterialShader
{
    CompiledMaterialShader
    (
        ShaderProgram&& prog,
        CompiledGeomVertexAttributes layout,
        const std::string& a_debug_name
    );

    CompiledMaterialShader(CompiledMaterialShader&&) = default;
    CompiledMaterialShader(const CompiledMaterialShader&) = delete;
    
    CompiledMaterialShader& operator=(CompiledMaterialShader&&) = default;
    void operator=(const CompiledMaterialShader&) = delete;

    std::string debug_name;

    // acutal program
    ShaderProgram program;
    CompiledGeomVertexAttributes mesh_layout;

    // uniforms
    CommonUniforms common;
    std::optional<LightUniforms> light;
    std::vector<Uniform> uniforms;
    
    // stored properties
    std::unordered_map<HashedString, PropertyIndex> name_to_array;
    CompiledProperties default_values;

    void
    use
    (
        const CompiledProperties& props,
        const Cache& cache,
        const CommonData& data,
        const LightData& light_data,
        LightStatus* ls
    ) const;
};



std::optional<CompiledMaterialShader>
load_material_shader(Engine* engine, Cache* cache, const Vfs& vfs, const std::string& shader_name, const LightParams& lp);

}
