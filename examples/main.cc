// standard headers
#include <string>
#include <string_view>
#include "tred/assert.h"
#include <vector>
#include <numeric>
#include <functional>
#include <array>
#include <map>
#include <set>

#include <fmt/core.h>

#include "glad/glad.h"
#include "tred/dependency_glm.h"

#include "tred/undef_windows.h"

#include "tred/dependency_imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

#include "mustache/mustache.hpp"


#include "tred/dependency_sdl.h"
#include "tred/opengl_debug.h"
#include "tred/cint.h"
#include "tred/geom.h"
#include "tred/geom.default.h"
#include "tred/texture.h"
#include "tred/uniform.h"
#include "tred/shader.h"
#include "tred/log.h"
#include "tred/geom.compiled.h"
#include "tred/texture.load.h"
#include "tred/windows.h"
#include "tred/input/system.h"
#include "tred/input/config.h"
#include "tred/input/table.h"
#include "tred/layer2.h"
#include "tred/spritebatch.h"
#include "tred/shader.source.h"
#include "tred/handle.h"
#include "tred/hash.string.h"
#include "tred/stdutils.h"

// resource headers
#include "shader_light.glsl.h"
#include "shader_material.glsl.h"
#include "container_diffuse.png.h"
#include "container_specular.png.h"

#include "sprites/cards.h"


#define USE_RENDERING 1 


struct Material
{
    Texture diffuse;
    Texture specular;

    glm::vec3 tint = glm::vec3{1.0f, 1.0f, 1.0f};
    float shininess = 32.0f;

    float specular_strength = 1.0f;

    Material(Texture&& d, Texture&& s)
        : diffuse(std::move(d))
        , specular(std::move(s))
    {
    }
};


struct MaterialUniforms
{
    Uniform diffuse;
    Uniform specular;

    Uniform tint;
    Uniform shininess;
    Uniform specular_strength;

    MaterialUniforms
    (
        ShaderProgram* shader,
        const std::string& base_name
    )
        : diffuse(shader->get_uniform(base_name + ".diffuse"))
        , specular(shader->get_uniform(base_name + ".specular"))
        , tint(shader->get_uniform(base_name + ".tint"))
        , shininess(shader->get_uniform(base_name + ".shininess"))
        , specular_strength(shader->get_uniform(base_name + ".specular_strength" ))
    {
        setup_textures(shader, {&diffuse, &specular});
    }

    void
    set_shader(ShaderProgram* shader, const Material& material) const
    {
        bind_texture(diffuse, material.diffuse);
        bind_texture(specular, material.specular);

        shader->set_vec3(tint, material.tint);
        shader->set_float(specular_strength, material.specular_strength);
        shader->set_float(shininess, material.shininess);
    }
};


struct DirectionalLight
{
    glm::vec3 position = glm::vec3{0.0f, 0.0f, 0.0f};

    float ambient_strength = 0.1f;
    glm::vec3 ambient =  glm::vec3{1.0f, 1.0f, 1.0f};
    glm::vec3 diffuse =  glm::vec3{1.0f, 1.0f, 1.0f};
    glm::vec3 specular = glm::vec3{1.0f, 1.0f, 1.0f};

    // todo(Gustav): make a property instead, function only here for demo purposes
    glm::vec3
    GetDirection() const
    {
        return glm::normalize(-position);
    }
};


struct DirectionalLightUniforms
{
    Uniform direction;
    Uniform ambient;
    Uniform diffuse;
    Uniform specular;

    DirectionalLightUniforms
    (
        const ShaderProgram& shader,
        const std::string& base_name
    )
    :
        direction(shader.get_uniform(base_name + ".normalized_direction")),
        ambient(shader.get_uniform(base_name + ".ambient")),
        diffuse(shader.get_uniform(base_name + ".diffuse")),
        specular(shader.get_uniform(base_name + ".specular"))
    {
    }

    void
    set_shader(const ShaderProgram& shader, const DirectionalLight& light) const
    {
        shader.set_vec3(direction, light.GetDirection());
        shader.set_vec3(ambient, light.ambient * light.ambient_strength);
        shader.set_vec3(diffuse, light.diffuse);
        shader.set_vec3(specular, light.specular);
    }
};


struct Attenuation
{
    float constant = 1.0f;
    float linear = 0.09f;
    float quadratic = 0.032f;
};

Attenuation zero_attenuation()
{
    return Attenuation{0.0f, 0.0f, 0.0f};
}

struct AttenuationUniforms
{
    Uniform constant;
    Uniform linear;
    Uniform quadratic;

    AttenuationUniforms
    (
        const ShaderProgram& shader,
        const std::string& base_name
    )
    :
        constant(shader.get_uniform(base_name + ".constant")),
        linear(shader.get_uniform(base_name + ".linear")),
        quadratic(shader.get_uniform(base_name + ".quadratic"))
    {
    }

    void
    set_shader(const ShaderProgram& shader, const Attenuation& att) const
    {
        shader.set_float(constant, att.constant);
        shader.set_float(linear, att.linear);
        shader.set_float(quadratic, att.quadratic);
    }
};


struct PointLight
{
    Attenuation attenuation;

    glm::vec3 position;

    float ambient_strength = 0.1f;
    glm::vec3 ambient =  glm::vec3{1.0f, 1.0f, 1.0f};
    glm::vec3 diffuse =  glm::vec3{1.0f, 1.0f, 1.0f};
    glm::vec3 specular = glm::vec3{1.0f, 1.0f, 1.0f};

    PointLight(const glm::vec3& p) : position(p) {}
};


struct PointLightUniforms
{
    AttenuationUniforms attenuation;
    Uniform position;
    Uniform ambient;
    Uniform diffuse;
    Uniform specular;

    PointLightUniforms
    (
        const ShaderProgram& shader,
        const std::string& base_name
    )
    :
        attenuation(shader, base_name + ".attenuation"),
        position(shader.get_uniform(base_name + ".position")),
        ambient(shader.get_uniform(base_name + ".ambient")),
        diffuse(shader.get_uniform(base_name + ".diffuse")),
        specular(shader.get_uniform(base_name + ".specular"))
    {
    }

    void
    set_shader(const ShaderProgram& shader, const PointLight& light) const
    {
        attenuation.set_shader(shader, light.attenuation);
        shader.set_vec3(position, light.position);
        shader.set_vec3(ambient, light.ambient * light.ambient_strength);
        shader.set_vec3(diffuse, light.diffuse);
        shader.set_vec3(specular, light.specular);
    }

    void
    turn_on_light(const ShaderProgram& shader, const PointLight& light) const
    {
        set_shader(shader, light);
    }

    void
    turn_off_light(const ShaderProgram& shader) const
    {
        auto dark = PointLight{glm::vec3{0.0f, 0.0f, 0.0f}};

        dark.attenuation = zero_attenuation();
        dark.ambient_strength = 0.0f;

        dark.ambient = {0.0f, 0.0f, 0.0f};
        dark.diffuse = {0.0f, 0.0f, 0.0f};
        dark.specular = {0.0f, 0.0f, 0.0f};

        set_shader(shader, dark);
    }
};


struct SpotLight
{
    Attenuation attenuation;

    glm::vec3 position = glm::vec3{0.0f, 0.0f, 0.0f};
    glm::vec3 direction = glm::vec3{0.0f, 0.0f, 0.0f};
    float cutoff = 12.5f;
    float outer_cutoff = 17.5;

    float ambient_strength = 0.1f;
    glm::vec3 ambient =  glm::vec3{1.0f, 1.0f, 1.0f};
    glm::vec3 diffuse =  glm::vec3{1.0f, 1.0f, 1.0f};
    glm::vec3 specular = glm::vec3{1.0f, 1.0f, 1.0f};
};


struct SpotLightUniforms
{
    AttenuationUniforms attenuation;
    Uniform position;
    Uniform direction;
    Uniform cos_cutoff;
    Uniform cos_outer_cutoff;
    Uniform ambient;
    Uniform diffuse;
    Uniform specular;

    SpotLightUniforms
    (
        const ShaderProgram& shader,
        const std::string& base_name
    )
    :
        attenuation(shader, base_name + ".attenuation"),
        position(shader.get_uniform(base_name + ".position")),
        direction(shader.get_uniform(base_name + ".direction")),
        cos_cutoff(shader.get_uniform(base_name + ".cos_cutoff")),
        cos_outer_cutoff(shader.get_uniform(base_name + ".cos_outer_cutoff")),
        ambient(shader.get_uniform(base_name + ".ambient")),
        diffuse(shader.get_uniform(base_name + ".diffuse")),
        specular(shader.get_uniform(base_name + ".specular"))
    {
    }

    void
    set_shader(const ShaderProgram& shader, const SpotLight& light) const
    {
        attenuation.set_shader(shader, light.attenuation);
        shader.set_vec3(position, light.position);
        shader.set_vec3(direction, light.direction);
        shader.set_float(cos_cutoff, cos(glm::radians(light.cutoff)));
        shader.set_float(cos_outer_cutoff, cos(glm::radians(light.outer_cutoff)));
        shader.set_vec3(ambient, light.ambient * light.ambient_strength);
        shader.set_vec3(diffuse, light.diffuse);
        shader.set_vec3(specular, light.specular);
    }

    void
    turn_on_light(const ShaderProgram& shader, const SpotLight& light) const
    {
        set_shader(shader, light);
    }

    void
    turn_off_light(const ShaderProgram& shader) const
    {
        // todo(Gustav): make constant
        auto dark = SpotLight{};

        dark.attenuation = zero_attenuation();

        dark.cutoff = 0.0f;
        dark.outer_cutoff = 0.0f;
        dark.ambient_strength = 0.0f;

        dark.ambient = {0.0f, 0.0f, 0.0f};
        dark.diffuse = {0.0f, 0.0f, 0.0f};
        dark.specular = {0.0f, 0.0f, 0.0f};

        set_shader(shader, dark);
    }
};


constexpr unsigned int NUMBER_OF_POINT_LIGHTS = 4;


constexpr auto UP = glm::vec3(0.0f, 1.0f, 0.0f);


struct CameraVectors
{
    glm::vec3 front;
    glm::vec3 right;
    glm::vec3 up;
    glm::vec3 position;

    CameraVectors(const glm::vec3& f, const glm::vec3& r, const glm::vec3& u, const glm::vec3& p)
        : front(f)
        , right(r)
        , up(u)
        , position(p)
    {
    }
};


struct Camera
{
    float fov = 45.0f;

    float near = 0.1f;
    float far = 100.0f;

    glm::vec3 position = glm::vec3{0.0f, 0.0f,  3.0f};

    float yaw = -90.0f;
    float pitch = 0.0f;

    CameraVectors create_vectors() const
    {
        const auto direction = glm::vec3
        {
            cos(glm::radians(yaw)) * cos(glm::radians(pitch)),
            sin(glm::radians(pitch)),
            sin(glm::radians(yaw)) * cos(glm::radians(pitch))
        };
        const auto front = glm::normalize(direction);
        const auto right = glm::normalize(glm::cross(front, UP));
        const auto up = glm::normalize(glm::cross(right, front));

        return {front, right, up, position};
    }
};


struct CompiledCamera
{
    glm::mat4 view;
    glm::vec3 position;

    CompiledCamera(const glm::mat4& v, const glm::vec3& p)
        : view(v)
        , position(p)
    {
    }
};


CompiledCamera compile_camera(const CameraVectors& camera)
{
    const auto view = glm::lookAt(camera.position, camera.position + camera.front, UP);
    return {view, camera.position};
}

float get_aspect_ratio(const Rectf& r)
{
    return r.get_width() / r.get_height();
}

float from01(float lower, float value, float upper)
{
    return value * (upper - lower) + lower;
}

float to01(float lower, float value, float upper)
{
    return (value - lower) / (upper - lower);
}

float remap(float ol, float ou, float f, float nl, float nu)
{
    const auto a = to01(ol, f, ou);
    return from01(nl, a, nu);
}


constexpr float plane_size = 20.0f;


using ShaderCompilerProperties = std::map<std::string, std::string>;

std::string
generate(std::string_view str, const ShaderCompilerProperties& properties)
{
    auto input = kainjow::mustache::mustache{std::string{str.begin(), str.end()}};
    input.set_custom_escape([](const std::string& s) { return s; });
    auto data = kainjow::mustache::data{};
    for(const auto& kv: properties)
    {
        data[kv.first] = kv.second;
    }
    return input.render(data);
}


void log_shader_error(const std::string& file, const shader::ShaderResult& res)
{
    for(const auto& e: res.log)
    {
        switch(e.type)
        {
        case shader::ShaderMessageType::info:
            LOG_INFO("{}({}): {}", file, e.line, e.message);
            break;
        case shader::ShaderMessageType::warning:
            LOG_WARNING("{}({}): {}", file, e.line, e.message);
            break;
        case shader::ShaderMessageType::error:
            LOG_ERROR("{}({}): {}", file, e.line, e.message);
            break;
        default:
            LOG_ERROR("INVALID LOG ENTRY {}({}): {}", file, e.line, e.message);
            break;
        }
    }
}

#if USE_RENDERING == 1
namespace rendering
{


enum class PointLightId : u64 {};
enum class SpotLightId : u64 {};
enum class TextureId : u64 {};
enum class CompiledMaterialShaderId : u64 {};

struct Vfs;
struct Cache;
struct Engine;

TextureId load_texture(Cache* cache, const Vfs& vfs, const std::string& path);
const Texture& get_texture(const Cache& cache, TextureId id);

enum class PropertyType
{
    float_type, vec3_type, vec4_type, texture_type
};

struct PropertyIndex
{
    PropertyType type;
    int index;
};
bool operator<(const PropertyIndex& lhs, const PropertyIndex& rhs)
{
    if(lhs.type == rhs.type)
    {
        return lhs.index < rhs.index;
    }
    else
    {
        return base_cast(lhs.type) < base_cast(rhs.type);
    }
}

struct LightParams
{
    int number_of_pointlights;
    int number_of_spotlights;
};

struct MaterialSourceProperty
{
    HashedString display_name; // used in ui and material references, example: "diffuse texture"
    std::string shader_uniform_ident; // glsl variable identifier, example: "uDiffuse"
    PropertyIndex index;
};

template<typename T>
struct UniformIndexAnd
{
    int index;
    T value;
};

template<typename T> UniformIndexAnd<T> make_uniform_and(int index, T val)
{
    return UniformIndexAnd<T>{index, val};
}

struct MaterialShaderSource
{
    // todo(Gustav): merge material shader source with cutstom frontmatter parsing so both shader and material shader can use the same frontmatter as base
    shader::ShaderSource source;
    bool apply_lightning;

    // todo(Gustav): specify sort order
    // todo(Gustav): specify blend mode
    // todo(Gustav): stencil test/write action

    std::vector<MaterialSourceProperty> properties;

    MaterialShaderSource(const shader::ShaderSource& src, bool al)
        : source(src)
        , apply_lightning(al)
    {
    }

    static MaterialShaderSource create_with_lights(const shader::ShaderSource& src) { return {src, true}; }
    static MaterialShaderSource create_unlit(const shader::ShaderSource& src) { return {src, false}; }

    // default values
    // example: this material exposes "diffuse" and unless specified it is white
    std::vector<float> floats;
    std::vector<glm::vec3> vec3s;
    std::vector<glm::vec4> vec4s;
    std::vector<std::string> textures;

    #define ADD_OP(FUNC_NAME, MEMBER, TYPE, ENUM)\
    MaterialShaderSource& FUNC_NAME(const HashedString& name, const std::string& uniform_name, const TYPE& v)\
    {\
        const auto index = Csizet_to_int(MEMBER.size());\
        MEMBER.emplace_back(v);\
        properties.emplace_back(MaterialSourceProperty{name, uniform_name, {ENUM, index}});\
        return *this;\
    }
    ADD_OP(with_float, floats, float, PropertyType::float_type)
    ADD_OP(with_vec3, vec3s, glm::vec3, PropertyType::vec3_type)
    ADD_OP(with_vec4, vec4s, glm::vec4, PropertyType::vec4_type)
    ADD_OP(with_texture, textures, std::string, PropertyType::texture_type)
    #undef ADD_OP
};


std::optional<MaterialShaderSource> load_material_shader_source(const Vfs& vfs, const std::string& path);


struct Material
{
    std::string shader;

    explicit Material(const std::string& shader_path): shader(shader_path) {}

    // custom properties
    // example: the "diffuse" property for all "box" meshes is "dirt.png"
    std::unordered_map<HashedString, PropertyIndex> name_to_array;
    std::vector<float> floats;
    std::vector<glm::vec3> vec3s;
    std::vector<glm::vec4> vec4s;
    std::vector<std::string> textures;

    #define ADD_OP(FUNC_NAME, MEMBER, TYPE, ENUM)\
    Material& FUNC_NAME(const HashedString& name, const TYPE& v)\
    {\
        const auto index = Csizet_to_int(MEMBER.size());\
        name_to_array.insert({name, PropertyIndex{ENUM, index}});\
        MEMBER.emplace_back(v);\
        return *this;\
    }
    ADD_OP(with_float, floats, float, PropertyType::float_type)
    ADD_OP(with_vec3, vec3s, glm::vec3, PropertyType::vec3_type)
    ADD_OP(with_vec4, vec4s, glm::vec4, PropertyType::vec4_type)
    ADD_OP(with_texture, textures, std::string, PropertyType::texture_type)
    #undef ADD_OP
};

struct LightData
{
    DirectionalLight directional_light;
    HandleVector64<PointLight, PointLightId> pointlights;
    HandleVector64<SpotLight, SpotLightId> spotlights;
};

// returns false if there were too many lights in the scene
template<typename TUniform, typename TData, typename TId>
[[nodiscard]] bool apply_data(const ShaderProgram& shader, const HandleVector64<TData, TId>& src, const std::vector<TUniform>& dst)
{
    std::size_t index = 0;

    for(const TData& data: src)
    {
        if(index>= dst.size()) { return false; }
        dst[index].turn_on_light(shader, data);
        index += 1;
    }

    for(;index<dst.size(); index+=1)
    {
        dst[index].turn_off_light(shader);
    }

    return true;
}

struct LightStatus
{
    bool applied_pointlights;
    bool applied_spotlights;

    static LightStatus create_no_error()
    {
        return {true, true};
    }
};

struct LightUniforms
{
    DirectionalLightUniforms direction_light;
    std::vector<PointLightUniforms> pointlights;
    std::vector<SpotLightUniforms> spotlights;

    LightUniforms(const ShaderProgram& shader, const LightParams& lp)
        : direction_light(shader, "uDirectionalLight")
    {
        assert(lp.number_of_spotlights == 1);

        for(int pointlight_index=0; pointlight_index<lp.number_of_pointlights; pointlight_index += 1)
        {
            const auto uniform_name = fmt::format("uPointLights[{}]", pointlight_index);
            pointlights.emplace_back(PointLightUniforms{shader, uniform_name});
        }
        spotlights.emplace_back(SpotLightUniforms{shader, "uSpotLight"});
    }

    [[nodiscard]] LightStatus set_shader(const ShaderProgram& prog, const LightData& data) const
    {
        direction_light.set_shader(prog, data.directional_light);
        const auto applied_pointlights = apply_data(prog, data.pointlights, pointlights);
        const auto applied_spotlights =  apply_data(prog, data.spotlights, spotlights);
        return
        {
            applied_pointlights,
            applied_spotlights
        };
    }
};

struct CompiledProperties
{
    std::vector<UniformIndexAnd<float>> floats;
    std::vector<UniformIndexAnd<glm::vec3>> vec3s;
    std::vector<UniformIndexAnd<glm::vec4>> vec4s;
    std::vector<UniformIndexAnd<TextureId>> textures;

    #define ADD_OP(FUNC_NAME, MEMBER, TYPE, ENUM)\
    void FUNC_NAME(PropertyIndex where, const TYPE& v)\
    {\
        assert(where.type == ENUM);\
        MEMBER[Cint_to_sizet(where.index)].value = v;\
    }
    ADD_OP(set_float, floats, float, PropertyType::float_type)
    ADD_OP(set_vec3, vec3s, glm::vec3, PropertyType::vec3_type)
    ADD_OP(set_vec4, vec4s, glm::vec4, PropertyType::vec4_type)
    ADD_OP(set_texture, textures, TextureId, PropertyType::texture_type)
    #undef ADD_OP

    void set_shader(const ShaderProgram& shader, const Cache& cache, const std::vector<Uniform>& uniforms) const
    {
        for(const auto& d: floats)
        {
            shader.set_float(uniforms[Cint_to_sizet(d.index)], d.value);
        }
        for(const auto& d: vec3s)
        {
            shader.set_vec3(uniforms[Cint_to_sizet(d.index)], d.value);
        }
        for(const auto& d: vec4s)
        {
            shader.set_vec4(uniforms[Cint_to_sizet(d.index)], d.value);
        }
        for(const auto& d: textures)
        {
            const auto& uniform = uniforms[Cint_to_sizet(d.index)];
            bind_texture(uniform, get_texture(cache, d.value));
            shader.set_texture(uniform);
        }
    }
};


struct CommonData
{
    glm::vec3 camera_position;
    glm::mat4 pv; // projection * view
    glm::mat4 model;
};

struct CommonUniforms
{
    CommonUniforms(const ShaderProgram& shader)
        : transform(shader.get_uniform("uTransform"))
        , model_transform(shader.get_uniform("uModelTransform"))
        , normal_matrix(shader.get_uniform("uNormalMatrix"))
        , view_position(shader.get_uniform("uViewPosition"))
    {
    }

    Uniform transform;
    Uniform model_transform;
    Uniform normal_matrix;
    Uniform view_position;

    void set_shader(const ShaderProgram& shader, const CommonData& data) const
    {
        shader.set_vec3(view_position, data.camera_position);
        shader.set_mat(transform, data.pv * data.model);
        shader.set_mat(model_transform, data.model);
        shader.set_mat(normal_matrix, glm::mat3(glm::transpose(glm::inverse(data.model))));
    }
};

struct CompiledMaterialShader
{
    CompiledMaterialShader(ShaderProgram&& prog, CompiledGeomVertexAttributes layout, const std::string& a_debug_name)
        : debug_name(a_debug_name)
        , program(std::move(prog))
        , mesh_layout(layout)
        , common(program)
    {
    }

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

    LightStatus use(const CompiledProperties& props, const Cache& cache, const CommonData& data, const LightData& light_data) const
    {
        program.use();
        common.set_shader(program, data);
        const LightStatus light_status = light ? light->set_shader(program, light_data) : LightStatus::create_no_error();
        props.set_shader(program, cache, uniforms);
        return light_status;
    }
};

CompiledVertexTypeList get_compile_attribute_layouts(Engine*, const ShaderVertexAttributes& layout)
{
    // todo(Gustav): add custom/global engine shader layouts here, example: outline shader or optional rimlight
    return compile_attribute_layouts({layout});
}

std::optional<CompiledMaterialShader> load_material_shader(Engine* engine, Cache* cache, const Vfs& vfs, const std::string& shader_name, const LightParams& lp)
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
        compile_defines.insert({"NUMBER_OF_POINT_LIGHTS", std::to_string(lp.number_of_pointlights)});
        compile_defines.insert({"NUMBER_OF_SPOT_LIGHTS", std::to_string(lp.number_of_spotlights)});
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

struct Mesh
{
    Material material;
    Geom geom;
};

CompiledMaterialShaderId load_compiled_material_shader(Engine* engine, const Vfs& vfs, Cache* cache, const std::string& path);
const CompiledMaterialShader& get_compiled_material_shader(const Cache& cache, CompiledMaterialShaderId id);

struct CompiledMaterial
{
    CompiledMaterialShaderId shader;
    CompiledProperties properties;

    // todo(Gustav): add util functions
    // override prop (hashedstring + prop)
    // clear prop (hashedstring)

    LightStatus use(const Cache& cache, const CommonData& data, const LightData& light_data) const
    {
        const auto& shader_data = get_compiled_material_shader(cache, shader);
        return shader_data.use(properties, cache, data, light_data);
    }
};


CompiledMaterial compile_material(Engine* engine, Cache* cache, const Vfs& vfs, const Material& mat)
{
    auto shader_id = load_compiled_material_shader(engine, vfs, cache, mat.shader);
    const auto& shader = get_compiled_material_shader(*cache, shader_id);

    auto properties = shader.default_values;

    for(const auto& nta: mat.name_to_array)
    {
        const auto found = shader.name_to_array.find(nta.first);
        if(found == shader.name_to_array.end())
        {
            LOG_ERROR("Invalid property {} in shader {}", nta.first, shader.debug_name);
            continue;
        }

        const auto& shader_index = found->second;
        const auto mat_index = nta.second;
        if(shader_index.type != mat_index.type)
        {
            LOG_ERROR("Type mismatch for {} in shader {}: {} vs {}", nta.first, shader.debug_name, shader_index.type, mat_index.type);
            continue;
        }

        switch(mat_index.type)
        {
        case PropertyType::float_type:   properties.floats  [Cint_to_sizet(shader_index.index)].value = mat.floats  [Cint_to_sizet(mat_index.index)]; break;
        case PropertyType::vec3_type:    properties.vec3s   [Cint_to_sizet(shader_index.index)].value = mat.vec3s   [Cint_to_sizet(mat_index.index)]; break;
        case PropertyType::vec4_type:    properties.vec4s   [Cint_to_sizet(shader_index.index)].value = mat.vec4s   [Cint_to_sizet(mat_index.index)]; break;
        case PropertyType::texture_type: properties.textures[Cint_to_sizet(shader_index.index)].value = load_texture(cache, vfs, mat.textures[Cint_to_sizet(mat_index.index)]); break;
        default:
            DIE("Unhandled type");
            break;
        }
    }

    return {shader_id, properties};
}

struct CompiledMesh
{
    CompiledMaterial material;
    CompiledGeom geom;

    LightStatus render(const Cache& cache, const CommonData& data, const LightData& light_data)
    {
        const auto ls = material.use(cache, data, light_data);
        geom.draw();
        return ls;
    }
};

CompiledMesh compile_mesh(Engine* engine, Cache* cache, const Vfs& vfs, const Mesh& mesh)
{
    auto material = compile_material(engine, cache, vfs, mesh.material);
    const auto& shader = get_compiled_material_shader(*cache, material.shader);
    auto geom = compile_geom(mesh.geom, shader.mesh_layout);

    return {material, std::move(geom)};
}



struct Vfs
{
    Vfs() = default;
    virtual ~Vfs() = default;

    Vfs(const Vfs&) = delete;
    Vfs(Vfs&&) = delete;
    
    void operator=(const Vfs&) = delete;
    void operator=(Vfs&&) = delete;

    virtual std::optional<MaterialShaderSource> load_material_shader_source(const std::string& path) const = 0;
    virtual std::optional<Texture> load_texture(const std::string& path) const = 0;
};


std::optional<MaterialShaderSource> load_material_shader_source(const Vfs& vfs, const std::string& path)
{
    return vfs.load_material_shader_source(path);
}


struct Cache
{
    TextureId get_or_load_default_texture()
    {
        // todo(Gustav): add default texture
        return *default_texture;
    }

    HandleVector64<Texture, TextureId> textures;
    HandleVector64<CompiledMaterialShader, CompiledMaterialShaderId> shaders;

    std::optional<TextureId> default_texture;
};

const Texture& get_texture(const Cache& cache, TextureId id)
{
    return cache.textures[id];
}

TextureId load_texture(Cache* cache, const Vfs& vfs, const std::string& path)
{
    auto texture = vfs.load_texture(path);
    if(texture)
    {
        return cache->textures.add(std::move(*texture));
    }

    // todo(Gustav): construct default texture with path? based on normal?
    LOG_WARNING("Unable to load texture {}", path);
    return cache->get_or_load_default_texture();
}

const LightParams& get_light_params(const Engine& engine);

CompiledMaterialShaderId load_compiled_material_shader(Engine* engine, const Vfs& vfs, Cache* cache, const std::string& path)
{
    auto shader = load_material_shader(engine, cache, vfs, path, get_light_params(*engine));
    if(shader)
    {
        return cache->shaders.add(std::move(*shader));
    }

    // todo(Gustav): load default shader?
    LOG_ERROR("Unable to load shader {}", path);
    DIE("unable to load shader");
    return static_cast<CompiledMaterialShaderId>(0);
}

const CompiledMaterialShader& get_compiled_material_shader(const Cache& cache, CompiledMaterialShaderId id)
{
    return cache.shaders[id];
}

enum class MeshId : u64 {};

struct Engine
{
    Engine(Vfs* a_vfs, const LightParams& alp)
        : vfs(a_vfs)
        , lp(alp)
    {
    }

    Vfs* vfs;
    LightParams lp;
    Cache cache;
    LightData lights;
    HandleVector64<CompiledMesh, MeshId> meshes;

    PointLightId add_point_light(const glm::vec3& p)
    {
        return lights.pointlights.add(PointLight{p});
    }

    void remove_point_light(PointLightId point)
    {
        lights.pointlights.remove(point);
    }

    MeshId add_mesh(const Mesh& mesh)
    {
        auto m = compile_mesh(this, &cache, *vfs, mesh);
        return meshes.add(std::move(m));
    }
    
    void render_mesh(MeshId mesh_id, const CommonData& data)
    {
        // todo(Gustav): how to handle render status? pass as a argument instead of a return as we cannot handle any of errors it returns
        (void)meshes[mesh_id].render(cache, data, lights);
    }

    /*
    void update_point_light_position(PointLightId point, const glm::vec3& position);

    SpotLightId add_spot_light();
    void remove_spot_light(SpotLightId spot);
    void update_spot_light();
    */
};

const LightParams& get_light_params(const Engine& engine)
{
    return engine.lp;
}

#if 0
using RenderId = std::uint64_t;

struct RenderCommand
{
    RenderId id;
};

bool operator<(const RenderCommand& lhs, const RenderCommand& rhs) { return lhs.id < rhs.id; }

// also called render queue but as a non-native speaker I hate typing queue
struct RenderList
{
    std::priority_queue<RenderCommand> commands;

    // todo(Gustav): test perf with a std::multiset or a std::vector (sorted in render)

    void begin()
    {
    }

    void add()
    {
    }

    void render()
    {
    }
};
#endif

} // namespace rendering
#endif


constexpr auto diffuse_color = HashedStringView{"Diffuse color"};
constexpr auto diffuse_texture = HashedStringView{"Diffuse texture"};
constexpr auto specular_texture = HashedStringView{"Specular texture"};
constexpr auto shininess_prop = HashedStringView{"Shininess"};
constexpr auto specular_strength_prop = HashedStringView{"Specular strength"};


struct FixedFileVfs : rendering::Vfs
{
    std::optional<rendering::MaterialShaderSource> load_material_shader_source(const std::string& path) const override
    {
        if(path == "default.glsl")
        {
            const auto src = shader::parse_shader_source(SHADER_MATERIAL_GLSL);
            log_shader_error(path, src);
            if(src.source.has_value() == false) { LOG_ERROR("Failed to parse shader file {}", path); return std::nullopt; }
            return rendering::MaterialShaderSource::create_with_lights(*src.source)
                .with_texture(diffuse_texture, "uMaterial.diffuse", "white.png")
                .with_texture(specular_texture, "uMaterial.specular", "no-specular.png")
                .with_vec3(diffuse_color, "uMaterial.tint", glm::vec3{1.0f, 0.0f, 0.0f})
                .with_float(shininess_prop, "uMaterial.shininess", 32.0f)
                .with_float(specular_strength_prop, "uMaterial.specular_strength", 1.0f)
                ;
        }
        if(path == "unlit.glsl")
        {
            const auto src = shader::parse_shader_source(SHADER_LIGHT_GLSL);
            log_shader_error(path, src);
            if(src.source.has_value() == false) { LOG_ERROR("Failed to parse shader file {}", path); return std::nullopt; }
            return rendering::MaterialShaderSource::create_unlit(*src.source)
                .with_vec3(diffuse_color, "uColor", glm::vec3{1.0f, 0.0f, 0.0f})
                ;
        }
        return std::nullopt;
    }

    std::optional<Texture> load_texture(const std::string& path) const override
    {
        if(path == "container_diffuse.png")
        {
            return load_image_from_embedded
            (
                CONTAINER_DIFFUSE_PNG,
                TextureEdge::repeat,
                TextureRenderStyle::smooth,
                Transparency::exclude
            );
        }
        if(path == "container_specular.png")
        {
            return load_image_from_embedded
            (
                CONTAINER_SPECULAR_PNG,
                TextureEdge::repeat,
                TextureRenderStyle::smooth,
                Transparency::exclude
            );
        }
        return std::nullopt;
    }
};

int
main(int, char**)
{
    const auto pi = 3.14f;

    ///////////////////////////////////////////////////////////////////////////
    // setup
    auto windows = setup();

    if(windows == nullptr)
    {
        LOG_ERROR("Failed to start");
        return 1;
    }

    bool running = true;

    constexpr std::string_view quit = "quit";
    constexpr std::string_view leftright = "leftright";
    constexpr std::string_view inout = "inout";
    constexpr std::string_view updown = "updown";
    constexpr std::string_view look_updown = "look_updown";
    constexpr std::string_view look_leftright = "look_leftright";

    auto sdl_input_loaded = input::load(input::config::InputSystem
    {
        {
            {quit, input::Range::within_zero_to_one},
            {leftright, input::Range::within_negative_one_to_positive_one},
            {inout, input::Range::within_negative_one_to_positive_one},
            {updown, input::Range::within_negative_one_to_positive_one},
            {look_updown, input::Range::infinite},
            {look_leftright, input::Range::infinite},
        },

        // controller setup (bind)
        {
            {
                "mouse+keyboard",
                {
                    input::config::KeyboardDefinition{input::KeyboardKey::return_key},
                    input::config::MouseDefinition{}
                },
                {
                    // keyboard
                    input::config::KeyBindDefinition{"quit", 0, input::KeyboardKey::escape},
                    input::config::TwoKeyBindDefinition{"leftright", 0, input::KeyboardKey::a, input::KeyboardKey::d},
                    input::config::TwoKeyBindDefinition{"inout", 0, input::KeyboardKey::s, input::KeyboardKey::w},
                    input::config::TwoKeyBindDefinition{"updown", 0, input::KeyboardKey::space, input::KeyboardKey::ctrl_left},

                    // mouse
                    input::config::AxisBindDefinition{"look_leftright", 1, input::Axis2::x, 0.1f},
                    input::config::AxisBindDefinition{"look_updown", 1, input::Axis2::y, 0.1f}
                }
            },
            {
                // probably bad to use a flightstick as a fps controller, but this is the only 'non-gamepad' joystick I own
                // speed link: black widow
                // identified as mega world usb controller
                "joystick",
                {
                    input::config::KeyboardDefinition{},
                    input::config::JoystickDefinition{4, "03000000b50700001703000010010000"}
                },
                {
                    // keyboard
                    input::config::KeyBindDefinition{"quit", 0, input::KeyboardKey::escape},

                    // joystick
                    input::config::AxisBindDefinition{"inout",     1, input::AxisType::general_axis, 0, 2},
                    input::config::AxisBindDefinition{"leftright", 1, input::AxisType::hat, 0, 1, 0.5f},
                    input::config::AxisBindDefinition{"updown",    1, input::AxisType::hat, 0, 2, 0.5f},
                    input::config::AxisBindDefinition{"look_leftright", 1, input::AxisType::general_axis, 0, 0, 50.0f, true},
                    input::config::AxisBindDefinition{"look_updown",    1, input::AxisType::general_axis, 0, 1, 50.0f, true}
                }
            },
            {
                "gamecontroller",
                {
                    input::config::KeyboardDefinition{},
                    input::config::GamecontrollerDefinition{}
                },
                {
                    // keyboard
                    input::config::KeyBindDefinition{"quit", 0, input::KeyboardKey::escape},

                    // gamecontroller
                    input::config::AxisBindDefinition{"inout",     1, input::GamecontrollerAxis::left_y, 1.0f, true},
                    input::config::AxisBindDefinition{"leftright", 1, input::GamecontrollerAxis::left_x},
                    input::config::TwoKeyBindDefinition{"updown", 1, input::GamecontrollerButton::trigger_left, input::GamecontrollerButton::trigger_right},
                    input::config::AxisBindDefinition{"look_leftright", 1, input::GamecontrollerAxis::right_x, 50.0f},
                    input::config::AxisBindDefinition{"look_updown",    1, input::GamecontrollerAxis::right_y, 50.0f, true}
                }
            },
        }
    });

    if(sdl_input_loaded == false)
    {
        LOG_ERROR("Unable to load input setup: {}", sdl_input_loaded.get_error());
        return -2;
    }

    auto sdl_input = std::move(*sdl_input_loaded.value);

    auto camera = ::Camera{};

    auto game_window = windows->add_window("TreD", {1280, 720});
    auto debug_window = windows->add_window("debug", {800, 600});

    auto cube_positions = std::vector<glm::vec3>
    {
        { 0.0f,  0.0f,  0.0f },
        { 2.0f,  5.0f, -5.0f},
        {-1.5f, -2.2f, -2.5f },
        {-3.8f, -2.0f, -5.3f},
        { 2.4f, -0.4f, -3.5f },
        {-1.7f,  3.0f, -7.5f },
        { 1.3f, -2.0f, -2.5f },
        { 1.5f,  2.0f, -2.5f },
        { 1.5f,  0.2f, -1.5f },
        {-1.3f,  1.0f, -1.5f }
    };

    auto directional_light = ::DirectionalLight{};
    auto point_lights = std::array<PointLight, NUMBER_OF_POINT_LIGHTS>
    {
        glm::vec3{ 0.7f,  0.2f,  2.0f},
        glm::vec3{ 2.3f, -3.3f, -4.0f},
        glm::vec3{-4.0f,  2.0f, -12.0f},
        glm::vec3{ 0.0f,  0.0f, -3.0f}
    };
    auto spot_light = ::SpotLight{};

#if USE_RENDERING == 1
    auto vfs = FixedFileVfs{};
    auto engine = rendering::Engine{&vfs, {NUMBER_OF_POINT_LIGHTS, 1}};

    const auto crate_mesh = engine.add_mesh
    ({
        rendering::Material{"default"}
            .with_texture(diffuse_texture, "crate_diffuse.png")
            .with_texture(specular_texture, "crate_specular.png")
            ,
        create_box_mesh(1.0f)
    });
    const auto light_mesh = engine.add_mesh
    ({
        rendering::Material{"unlit"},
        create_box_mesh(0.2f)
    });
    const auto plane_mesh = engine.add_mesh
    ({
        rendering::Material{"default"}
            .with_texture(diffuse_texture, "crate_diffuse.png")
            .with_texture(specular_texture, "crate_specular.png")
            ,
        create_plane_mesh(plane_size, plane_size)
    });
#else
    ///////////////////////////////////////////////////////////////////////////
    // shader layout
    const auto material_shader_source_result = shader::parse_shader_source(SHADER_MATERIAL_GLSL);
    log_shader_error("shader_material", material_shader_source_result);
    if(material_shader_source_result.source.has_value() == false) { return -1;}
    const auto material_shader_source = *material_shader_source_result.source;

    const auto material_shader_layout = material_shader_source.layout;

    auto material_layout_compiler = compile_attribute_layouts({material_shader_layout});
    const auto material_mesh_layout = material_layout_compiler.get_mesh_layout();
    const auto compiled_layout = material_layout_compiler.compile_shader_layout(material_shader_layout);

    const auto light_shader_source_result = shader::parse_shader_source(SHADER_LIGHT_GLSL);
    log_shader_error("shader_light", light_shader_source_result);
    if(light_shader_source_result.source.has_value() == false) { return -1;}
    const auto light_shader_source = *light_shader_source_result.source;

    const auto light_shader_layout = light_shader_source.layout;
    auto light_compiler = compile_attribute_layouts({light_shader_layout});
    const auto light_mesh_layout = light_compiler.get_mesh_layout();
    const auto compiled_light_layout = light_compiler.compile_shader_layout(light_shader_layout);

    ///////////////////////////////////////////////////////////////////////////
    // shaders
    const auto shader_options = ShaderCompilerProperties
    {
        {"NUMBER_OF_POINT_LIGHTS", std::to_string(NUMBER_OF_POINT_LIGHTS)}
    };
    auto shader = ::ShaderProgram
    {
        generate(material_shader_source.vertex, shader_options),
        generate(material_shader_source.fragment, shader_options),
        compiled_layout
    };
    const auto uni_color = shader.get_uniform("uColor");
    const auto uni_transform = shader.get_uniform("uTransform");
    const auto uni_model_transform = shader.get_uniform("uModelTransform");
    const auto uni_normal_matrix = shader.get_uniform("uNormalMatrix");
    const auto uni_view_position = shader.get_uniform("uViewPosition");
    const auto uni_material = MaterialUniforms{&shader, "uMaterial"};
    const auto uni_directional_light = DirectionalLightUniforms{shader, "uDirectionalLight"};
    const auto uni_point_lights = std::array<PointLightUniforms, NUMBER_OF_POINT_LIGHTS>
    {
        PointLightUniforms{shader, "uPointLights[0]"},
        PointLightUniforms{shader, "uPointLights[1]"},
        PointLightUniforms{shader, "uPointLights[2]"},
        PointLightUniforms{shader, "uPointLights[3]"}
    };
    const auto uni_spot_light = SpotLightUniforms{shader, "uSpotLight"};

    auto light_shader = ::ShaderProgram{light_shader_source.vertex, light_shader_source.fragment, compiled_light_layout};
    const auto uni_light_transform = light_shader.get_uniform("uTransform");
    const auto uni_light_color = light_shader.get_uniform("uColor");

    ///////////////////////////////////////////////////////////////////////////
    // model
    const auto mesh = compile_geom(create_box_mesh(1.0f), material_mesh_layout);
    const auto light_mesh = compile_geom(create_box_mesh(0.2f), light_mesh_layout);
    const auto plane_mesh = compile_geom(create_plane_mesh(plane_size, plane_size), material_mesh_layout);

    auto cube_color = glm::vec4{1.0f};

    auto material = ::Material
    {
        load_image_from_embedded
        (
            CONTAINER_DIFFUSE_PNG,
            TextureEdge::repeat,
            TextureRenderStyle::smooth,
            Transparency::exclude
        ),
        load_image_from_embedded
        (
            CONTAINER_SPECULAR_PNG,
            TextureEdge::repeat,
            TextureRenderStyle::smooth,
            Transparency::exclude
        )
    };
#endif

    auto cards = Texture{::cards::load_texture()};

    float time = 0.0f;
    bool animate = false;
    bool use_custom_hud = false;
    float hud_lerp = 1.0f; // 1= black bars, 0=no black bars

    windows->set_render
    (
        game_window,
        [&](const RenderCommand& rc)
        {
            // draw 3d world
            // clear screen
            const auto layout = LerpData
            {
                {ViewportStyle::extended, 800.0f, 600.0f},
                {ViewportStyle::black_bars, 800.0f, 600.0f},
                hud_lerp
            };
            
            // clear to black
            if(hud_lerp > 0.0f)
            {
                rc.clear(glm::vec3{0.0f}, LayoutData{ViewportStyle::extended, 800.0f, 600.0f});
            }

            // clear background
            rc.clear({0.2f, 0.3f, 0.3f}, layout);

            {
                auto l3 = with_layer3(rc, layout);

                const auto aspect_ratio = get_aspect_ratio(l3.viewport_aabb_in_worldspace);
                const glm::mat4 projection = glm::perspective(glm::radians(camera.fov), aspect_ratio, camera.near, camera.far);

                // todo(Gustav): move clear to rc
                glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

                const auto compiled_camera = compile_camera(camera.create_vectors());
            
                const auto view = compiled_camera.view;

                const auto pv = projection * view;

#if USE_RENDERING == 1
                // todo(Gustav): set light data
                // refactor to use immediate mode

                for(unsigned int i=0; i<cube_positions.size(); i+=1)
                {
                    const auto angle = 20.0f * static_cast<float>(i);
                    const auto model = glm::rotate
                    (
                        glm::translate(glm::mat4(1.0f), cube_positions[i]),
                        time + glm::radians(angle),
                        i%2 == 0
                        ? glm::vec3{1.0f, 0.3f, 0.5f}
                        : glm::vec3{0.5f, 1.0f, 0.0f}
                    );
                    engine.render_mesh(crate_mesh, {compiled_camera.position, pv, model});
                }

                // draw lights
                for(const auto& pl: point_lights)
                {
                    // light_shader.set_vec3(uni_light_color, pl.diffuse);
                    const auto model = glm::translate(glm::mat4(1.0f), pl.position);
                    engine.render_mesh(light_mesh, {compiled_camera.position, pv, model});
                }
                
                {
                    const auto model = glm::translate(glm::mat4(1.0f), {0.0f, -3.5f, 0.0f});
                    engine.render_mesh(plane_mesh, {compiled_camera.position, pv, model});
                }
#else
                for(unsigned int i=0; i<NUMBER_OF_POINT_LIGHTS; i+=1)
                {
                    light_shader.use();
                    light_shader.set_vec3(uni_light_color, point_lights[i].diffuse);
                    {
                        const auto model = glm::translate(glm::mat4(1.0f), point_lights[i].position);
                        light_shader.set_mat(uni_light_transform, pv * model);
                    }
                    light_mesh.draw();
                }

                shader.use();
                shader.set_vec4(uni_color, cube_color);
                uni_material.set_shader(&shader, material);
                uni_directional_light.set_shader(shader, directional_light);
                uni_spot_light.set_shader(shader, spot_light);
                for(unsigned int i=0; i<NUMBER_OF_POINT_LIGHTS; i+=1)
                {
                    uni_point_lights[i].set_shader(shader, point_lights[i]);
                }
                shader.set_vec3(uni_view_position, compiled_camera.position);

                for(unsigned int i=0; i<cube_positions.size(); i+=1)
                {
                    const auto angle = 20.0f * static_cast<float>(i);
                    {
                        const auto model = glm::rotate
                        (
                            glm::translate(glm::mat4(1.0f), cube_positions[i]),
                            time + glm::radians(angle),
                            i%2 == 0
                            ? glm::vec3{1.0f, 0.3f, 0.5f}
                            : glm::vec3{0.5f, 1.0f, 0.0f}
                        );
                        shader.set_mat(uni_transform, pv * model);
                        shader.set_mat(uni_model_transform, model);
                        shader.set_mat(uni_normal_matrix, glm::mat3(glm::transpose(glm::inverse(model))));
                    }
                    mesh.draw();
                }

                {
                    const auto model = glm::translate(glm::mat4(1.0f), {0.0f, -3.5f, 0.0f});
                    shader.set_mat(uni_transform, pv * model);
                    shader.set_mat(uni_model_transform, model);
                    shader.set_mat(uni_normal_matrix, glm::mat3(glm::transpose(glm::inverse(model))));
                    plane_mesh.draw();
                }
#endif
            }

            // draw hud
            {
                auto l2 = use_custom_hud
                    ? with_layer2(rc, LayoutData{ViewportStyle::extended, 800.0f, 600.0f})
                    : with_layer2(rc, layout)
                    ;

                constexpr auto card_sprite = Cint_to_float(::cards::back).zero().set_height(90);

                l2.batch->quad(&cards, card_sprite.translate(10, 10), ::cards::hearts[2]);
            }
        }
    );

    windows->set_render
    (
        debug_window,
        [&](const RenderCommand& rc)
        {
            const auto debug_layout = LayoutData{ViewportStyle::extended, 800.0f, 600.0f};
            rc.clear({0.1f, 0.1f, 0.7f}, debug_layout);
            {
                auto l2 = with_layer2(rc, debug_layout);


                auto tr = [](const glm::vec3& pos) -> Rectf
                {
                    constexpr float half_size = plane_size / 2.0f;
                    constexpr auto card_sprite = Cint_to_float(::cards::back).zero().set_height(90);
                    return card_sprite.translate
                    (
                        remap(-half_size, half_size, pos.x, 0.0f, 800.0f-card_sprite.get_width()),
                        remap(-half_size, half_size, pos.z, 0.0f, 600.0f-card_sprite.get_height())
                    );
                };

                std::size_t i = 0;
                for(const auto p: cube_positions)
                {
                    l2.batch->quad(&cards, tr(p), ::cards::hearts[i]);
                    i = (i+1) % ::cards::hearts.size();
                }
                l2.batch->quad(&cards, tr(camera.position), ::cards::back);
            }
        }
    );

    int rendering_mode = 0;
    const auto set_rendering_mode = [&rendering_mode]()
    {
        switch(rendering_mode)
        {
            case 0: glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); break;
            case 1: glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); break;
            case 2: glPolygonMode(GL_FRONT_AND_BACK, GL_POINT); break;
            default: DIE("invalid rendering_mode"); break;
        }
    };

    windows->set_imgui
    (
        [&]()
        {

            if(ImGui::Begin("Debug"))
            {
                if(ImGui::Button("Quit")) { running = false; }
                ImGui::SameLine();
                ImGui::Checkbox("Animate?", &animate);
                ImGui::SameLine();
                ImGui::SliderFloat("Time", &time, 0.0f, 2 * pi);
                
                ImGui::SliderFloat("Black bars?", &hud_lerp, 0.0f, 1.0f);
                ImGui::SameLine();
                ImGui::Checkbox("Custom hud?", &use_custom_hud);

                if(ImGui::Combo("Rendering mode", &rendering_mode, "Fill\0Line\0Point\0"))
                {
                    set_rendering_mode();
                }
                ImGui::DragFloat("FOV", &camera.fov, 0.1f, 1.0f, 145.0f);
                if (ImGui::CollapsingHeader("Lights"))
                {
                    const auto ui_attenuation = [](Attenuation* a)
                    {
                        ImGui::DragFloat("Attenuation constant", &a->constant, 0.01f);
                        ImGui::DragFloat("Attenuation linear", &a->linear, 0.01f);
                        ImGui::DragFloat("Attenuation quadratic", &a->quadratic, 0.01f);
                    };

                    const auto ui_directional = [](DirectionalLight* light)
                    {
                        ImGui::DragFloat("Strength", &light->ambient_strength, 0.01f);
                        ImGui::ColorEdit3("Ambient", glm::value_ptr(light->ambient));
                        ImGui::ColorEdit3("Diffuse", glm::value_ptr(light->diffuse));
                        ImGui::ColorEdit3("Specular", glm::value_ptr(light->specular));
                        ImGui::DragFloat3("Position", glm::value_ptr(light->position), 0.01f);
                    };

                    const auto ui_point = [&ui_attenuation](PointLight* light)
                    {
                        ui_attenuation(&light->attenuation);
                        ImGui::DragFloat("Strength", &light->ambient_strength, 0.01f);
                        ImGui::ColorEdit3("Ambient", glm::value_ptr(light->ambient));
                        ImGui::ColorEdit3("Diffuse", glm::value_ptr(light->diffuse));
                        ImGui::ColorEdit3("Specular", glm::value_ptr(light->specular));
                        ImGui::DragFloat3("Position", glm::value_ptr(light->position), 0.01f);
                    };

                    const auto ui_spot = [&ui_attenuation](SpotLight* light)
                    {
                        ui_attenuation(&light->attenuation);

                        ImGui::DragFloat("Ambient strength", &light->ambient_strength, 0.01f);
                        ImGui::ColorEdit3("Ambient", glm::value_ptr(light->ambient));
                        ImGui::ColorEdit3("Diffuse", glm::value_ptr(light->diffuse));
                        ImGui::ColorEdit3("Specular", glm::value_ptr(light->specular));

                        ImGui::DragFloat("Cutoff", &light->cutoff, 0.1f);
                        ImGui::DragFloat("Outer cutoff", &light->outer_cutoff, 0.1f);
                    };

                    if(ImGui::CollapsingHeader("Directional"))
                    {
                        ImGui::PushID("directional light");
                        ui_directional(&directional_light);
                        ImGui::PopID();
                    }
                    if(ImGui::CollapsingHeader("Spot"))
                    {
                        ImGui::PushID("spot light");
                        ui_spot(&spot_light);
                        ImGui::PopID();
                    }
                    if(ImGui::CollapsingHeader("Point"))
                    {
                        for(unsigned int i=0; i<NUMBER_OF_POINT_LIGHTS; i+=1)
                        {
                            ImGui::PushID(static_cast<int>(i));
                            ui_point(&point_lights[i]);
                            ImGui::PopID();
                        }
                    }
                }

                if (ImGui::CollapsingHeader("Cubes"))
                {
#if USE_RENDERING == 0
                    ImGui::ColorEdit4("Cube colors", glm::value_ptr(cube_color));
                    ImGui::ColorEdit3("Tint color", glm::value_ptr(material.tint));
                    ImGui::DragFloat("Specular strength", &material.specular_strength, 0.01f);
                    ImGui::DragFloat("Shininess", &material.shininess, 1.0f, 2.0f, 256.0f);
#endif
                    for(auto& cube: cube_positions)
                    {
                        ImGui::PushID(&cube);
                        ImGui::DragFloat3("", glm::value_ptr(cube), 0.01f);
                        ImGui::PopID();
                    }
                }
            }
            ImGui::End();
        }
    );

    auto player = sdl_input.add_player();
    auto get = [](const input::Table& table, std::string_view name, float d=0.0f) -> float
    {
        const auto found = table.data.find(std::string{name});
        if(found == table.data.end()) { return d; }
        return found->second;
    };

    return main_loop(input::unit_discovery::find_highest, std::move(windows), &sdl_input, [&](float dt) -> bool
    {
        input::Table table;
        sdl_input.update_table(player, &table, dt);

        if(animate)
        {
            time += dt;

            if(time > 2*pi)
            {
                time -= 2*pi;
            }
        }

        if(get(table, quit, 1.0f) > 0.5f)
        {
            running = false;
        }

        const auto v = camera.create_vectors();

        const auto input_inout = get(table, inout);
        const auto input_leftright = get(table, leftright);
        const auto input_updown = get(table, updown);

        const auto camera_movement
            = v.front * input_inout
            + v.right * input_leftright
            + v.up * input_updown
            ;

        constexpr bool input_shift = false; // todo(Gustav): expose toggleables
        const auto camera_speed = 3 * dt * (input_shift ? 2.0f : 1.0f);
        camera.position += camera_speed * camera_movement;

        // LOG_INFO("mouse: {} {}", get(table, look_leftright), get(table, look_updown));
        const float sensitivity = 1.0f;
        camera.yaw += get(table, look_leftright) * sensitivity;
        camera.pitch += get(table, look_updown) * sensitivity;
        if(camera.pitch >  89.0f) camera.pitch =  89.0f;
        if(camera.pitch < -89.0f) camera.pitch = -89.0f;

        return running;
    });
}

