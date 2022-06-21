#include "tred/render/compiledmaterialshader.h"

#include "tred/assert.h"
#include "tred/cint.h"

#include "tred/render/light.params.h"
#include "tred/render/material.shader.source.h"
#include "tred/render/shader.template.h"


namespace render
{


CommonUniforms::CommonUniforms(const ShaderProgram& shader)
    : transform(shader.get_uniform("uTransform"))
    , model_transform(shader.get_uniform("uModelTransform"))
    , normal_matrix(shader.get_uniform("uNormalMatrix"))
    , view_position(shader.get_uniform("uViewPosition"))
{
}

void CommonUniforms::set_shader(const ShaderProgram& shader, const CommonData& data) const
{
    shader.set_vec3(view_position, data.camera_position);
    shader.set_mat(transform, data.pv * data.model);
    shader.set_mat(model_transform, data.model);
    shader.set_mat(normal_matrix, glm::mat3(glm::transpose(glm::inverse(data.model))));
}


CompiledMaterialShader::CompiledMaterialShader
(
    ShaderProgram&& prog,
    CompiledGeomVertexAttributes layout,
    const std::string& a_debug_name
)
    : debug_name(a_debug_name)
    , program(std::move(prog))
    , mesh_layout(layout)
    , common(program)
{
}


void
CompiledMaterialShader::use
(
    const CompiledProperties& props,
    const Cache& cache,
    const CommonData& data,
    const LightData& light_data,
    LightStatus* ls
) const
{
    program.use();
    common.set_shader(program, data);
    if(light)
    {
        light->set_shader(program, light_data, ls);
    }
    props.set_shader(program, cache, uniforms);
}




// todo(Gustav): move to a header
CompiledVertexTypeList get_compile_attribute_layouts(Engine*, const ShaderVertexAttributes& layout);




template<typename T> UniformIndexAnd<T> make_uniform_and(int index, T val)
{
    return UniformIndexAnd<T>{index, val};
}


std::optional<CompiledMaterialShader>
load_material_shader(Engine* engine, Cache* cache, const Vfs& vfs, const std::string& shader_name, const LightParams& lp)
{
    auto loaded_shader_source = load_material_shader_source(vfs, shader_name);
    if(!loaded_shader_source) { return std::nullopt; }
    const MaterialShaderSource& shader_source = std::move(*loaded_shader_source);

    auto layout_compiler = get_compile_attribute_layouts(engine, shader_source.source.layout);
    const auto mesh_layout = layout_compiler.get_mesh_layout();
    const auto compiled_layout = layout_compiler.compile_shader_layout(shader_source.source.layout);

    auto compile_defines = ShaderCompilerProperties{};
    if(shader_source.apply_lightning)
    {
        compile_defines.insert({"NUMBER_OF_DIRECTIONAL_LIGHTS", std::to_string(lp.number_of_directional_lights)});
        compile_defines.insert({"NUMBER_OF_POINT_LIGHTS", std::to_string(lp.number_of_point_lights)});
        compile_defines.insert({"NUMBER_OF_SPOT_LIGHTS", std::to_string(lp.number_of_spot_lights)});
    }
    auto ret = CompiledMaterialShader
    {
        {
            generate(shader_source.source.vertex, compile_defines),
            generate(shader_source.source.fragment, compile_defines),
            compiled_layout
        },
        mesh_layout,
        shader_name
    };
    
    auto texture_uniform_indices = std::vector<std::size_t>{};

    std::map<PropertyIndex, int> array_to_uniform;

    for(const auto& prop: shader_source.properties)
    {
        if(prop.index.type == PropertyType::texture_type)
        {
            texture_uniform_indices.emplace_back(ret.uniforms.size());
        }
        const auto uniform_index = ret.uniforms.size();
        ret.uniforms.emplace_back(ret.program.get_uniform(prop.shader_uniform_ident));
        ret.name_to_array.insert({prop.display_name, prop.index});
        array_to_uniform.insert({prop.index, Csizet_to_int(uniform_index)});
    }

    // load default values
#define COPY(PROP, ENUM) do { int index = 0; for(const auto& d: shader_source.PROP) { auto found = array_to_uniform.find({ENUM, index}); ASSERT(found != array_to_uniform.end()); ret.default_values.PROP.emplace_back(make_uniform_and(found->second, d)); index+=1;}} while(false)
    COPY(floats, PropertyType::float_type);
    COPY(vec3s, PropertyType::vec3_type);
    COPY(vec4s, PropertyType::vec4_type);
#undef COPY
    {
        int index = 0;
        for(const auto& path: shader_source.textures)
        {
            auto found = array_to_uniform.find({PropertyType::texture_type, index});
            ASSERT(found != array_to_uniform.end()); 
            ret.default_values.textures.emplace_back(make_uniform_and(found->second, load_texture(cache, vfs, path)));
            index += 1;
        }
    }

    {
        auto textures = std::vector<Uniform*>{};
        for(auto index: texture_uniform_indices)
        {
            textures.emplace_back(&ret.uniforms[index]);
        }
        setup_textures(&ret.program, textures);
    }

    if(shader_source.apply_lightning)
    {
        ret.light = LightUniforms{ret.program, lp};
    }
    return ret;
}



}
