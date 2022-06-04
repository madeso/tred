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

#define OLD_INPUT 0

constexpr auto white3 = glm::vec3{1.0f, 1.0f, 1.0f};
constexpr auto black3 = glm::vec3{0.0f, 0.0f, 0.0f};

struct DirectionalLight
{
    // todo(Gustav): remove this?
    glm::vec3 position = glm::vec3{0.0f, 0.0f, 0.0f};

    float ambient_strength;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    DirectionalLight()
        : ambient_strength(0.1f)
        , ambient(white3)
        , diffuse(white3)
        , specular(white3)
    {
    }

    DirectionalLight
    (
        float as,
        const glm::vec3& a,
        const glm::vec3& d,
        const glm::vec3& s
    )
        : ambient_strength(as)
        , ambient(a)
        , diffuse(d)
        , specular(s)
    {
    }

    static DirectionalLight create_no_light()
    {
        return DirectionalLight{0.0f, black3, black3, black3};
    }

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

    void turn_on_light(const ShaderProgram& shader, const DirectionalLight& light) const
    {
        set_shader(shader, light);
    }

    void turn_off_light(const ShaderProgram& shader) const
    {
        set_shader(shader, DirectionalLight::create_no_light());
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
constexpr unsigned int NUMBER_OF_SPOT_LIGHTS = 1;
constexpr unsigned int NUMBER_OF_DIRECTIONAL_LIGHTS = 1;


constexpr auto UP = glm::vec3(0.0f, 1.0f, 0.0f);


struct CameraVectors
{
    glm::vec3 front;
    glm::vec3 right;
    glm::vec3 up;
    glm::vec3 position;
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


namespace rendering
{


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
    int number_of_directional_lights;
    int number_of_point_lights;
    int number_of_spot_lights;
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
    std::vector<DirectionalLight> directional_lights;
    std::vector<PointLight> point_lights;
    std::vector<SpotLight> spot_lights;

    void clear()
    {
        directional_lights.clear();
        point_lights.clear();
        spot_lights.clear();
    }
};

// returns false if there were too many lights in the scene
template<typename TUniform, typename TData>
void apply_data(const ShaderProgram& shader, const std::vector<TData>& src, const std::vector<TUniform>& dst, bool* applied)
{
    std::size_t index = 0;

    for(const TData& data: src)
    {
        if(index>= dst.size()) { *applied = false; return; }
        dst[index].turn_on_light(shader, data);
        index += 1;
    }

    for(;index<dst.size(); index+=1)
    {
        dst[index].turn_off_light(shader);
    }
}

struct LightStatus
{
    bool applied_directional_lights;
    bool applied_point_lights;
    bool applied_spot_lights;

    static LightStatus create_no_error()
    {
        return {true, true, true};
    }
};

struct LightUniforms
{
    std::vector<DirectionalLightUniforms> directional_lights;
    std::vector<PointLightUniforms> point_lights;
    std::vector<SpotLightUniforms> spot_lights;

    LightUniforms(const ShaderProgram& shader, const LightParams& lp)
    {
        assert(lp.number_of_directional_lights == 1);
        directional_lights.emplace_back(DirectionalLightUniforms(shader, "uDirectionalLight"));

        assert(lp.number_of_spot_lights == 1);
        spot_lights.emplace_back(SpotLightUniforms{shader, "uSpotLight"});

        for(int point_light_index=0; point_light_index<lp.number_of_point_lights; point_light_index += 1)
        {
            const auto uniform_name = fmt::format("uPointLights[{}]", point_light_index);
            point_lights.emplace_back(PointLightUniforms{shader, uniform_name});
        }
    }

    void set_shader(const ShaderProgram& prog, const LightData& data, LightStatus* ls) const
    {
        apply_data(prog, data.directional_lights, directional_lights, &ls->applied_directional_lights);
        apply_data(prog, data.point_lights, point_lights, &ls->applied_point_lights);
        apply_data(prog, data.spot_lights, spot_lights, &ls->applied_spot_lights);
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

    void use(const CompiledProperties& props, const Cache& cache, const CommonData& data, const LightData& light_data, LightStatus* ls) const
    {
        program.use();
        common.set_shader(program, data);
        if(light)
        {
            light->set_shader(program, light_data, ls);
        }
        props.set_shader(program, cache, uniforms);
    }
};

CompiledVertexTypeList get_compile_attribute_layouts(Engine*, const ShaderVertexAttributes& layout);

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

    // todo(Gustav): add util functions (that also take index) and clear prop (hashedstring+index)

    #define ADD_OP(FUNC_NAME, MEMBER, TYPE, ENUM)\
    void set_##FUNC_NAME##_by_lookup(const Cache& cache, const HashedString& name, const TYPE& v)\
    {\
        const auto& cms = get_compiled_material_shader(cache, shader);\
        const auto found = cms.name_to_array.find(name);\
        ASSERT(found != cms.name_to_array.end());\
        properties.set_##FUNC_NAME(found->second, v);\
    }
    ADD_OP(float, floats, float, PropertyType::float_type)
    ADD_OP(vec3, vec3s, glm::vec3, PropertyType::vec3_type)
    ADD_OP(vec4, vec4s, glm::vec4, PropertyType::vec4_type)
    ADD_OP(texture, textures, TextureId, PropertyType::texture_type)
    #undef ADD_OP

    void use(const Cache& cache, const CommonData& data, const LightData& light_data, LightStatus* ls) const
    {
        const auto& shader_data = get_compiled_material_shader(cache, shader);
        return shader_data.use(properties, cache, data, light_data, ls);
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


void render_geom_with_material
(
    const CompiledGeom& geom,
    const CompiledMaterial& material,
    const Cache& cache,
    const CommonData& data,
    const LightData& light_data,
    LightStatus* ls
)
{
    material.use(cache, data, light_data, ls);
    geom.draw();
}

struct CompiledMesh
{
    CompiledGeom geom;
    CompiledMaterial material;
};

CompiledMesh compile_mesh(Engine* engine, Cache* cache, const Vfs& vfs, const Mesh& mesh)
{
    auto material = compile_material(engine, cache, vfs, mesh.material);
    const auto& shader = get_compiled_material_shader(*cache, material.shader);
    auto geom = compile_geom(mesh.geom, shader.mesh_layout);

    return {std::move(geom), material};
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

enum class GeomId : u64 {};
enum class MaterialId : u64 {};


struct RenderCommand
{
    GeomId geom_id;
    MaterialId material_id;
    glm::mat4 model;
};

// bool operator<(const RenderCommand& lhs, const RenderCommand& rhs) { return lhs.id < rhs.id; }

// also called render queue but as a non-native speaker I hate typing queue
struct RenderList
{
    // todo(Gustav): sort commands!
    // std::priority_queue<RenderCommand> commands;
    // todo(Gustav): test perf with a std::multiset or a std::vector (sorted in render)
    std::vector<RenderCommand> commands;

    LightData lights;
    glm::vec3 camera_position;
    glm::mat4 projection_view;
    LightStatus light_status;
    std::optional<MaterialId> global_shader;

    bool is_rendering = false;

    void begin_perspective(float aspect_ratio, const Camera& camera, std::optional<MaterialId> the_global_shader)
    {
        ASSERT(is_rendering == false);
        is_rendering = true;

        const glm::mat4 projection = glm::perspective(glm::radians(camera.fov), aspect_ratio, camera.near, camera.far);
        const auto compiled_camera = compile_camera(camera.create_vectors());
        const auto view = compiled_camera.view;
        const auto pv = projection * view;

        camera_position = compiled_camera.position;
        projection_view = pv;
        global_shader = the_global_shader;

        light_status = LightStatus::create_no_error();
        commands.clear();
        lights.clear();
    }

    void add_directional_light(const DirectionalLight& d)
    {
        ASSERT(is_rendering);
        lights.directional_lights.emplace_back(d);
    }

    void add_point_light(const PointLight& p)
    {
        ASSERT(is_rendering);
        lights.point_lights.emplace_back(p);
    }

    void add_spot_light(const SpotLight& s)
    {
        ASSERT(is_rendering);
        lights.spot_lights.emplace_back(s);
    }

    void add_mesh(GeomId geom_id, MaterialId material_id, const glm::mat4& model)
    {
        ASSERT(is_rendering);
        commands.emplace_back(RenderCommand{geom_id, global_shader.value_or(material_id), model});
    }

    void end()
    {
        ASSERT(is_rendering);
        is_rendering = false;
    }
};


struct AddedMesh
{
    GeomId geom;
    MaterialId material;
};

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
    
    HandleVector64<CompiledGeom, GeomId> geoms;
    HandleVector64<CompiledMaterial, MaterialId> materials;
    std::vector<VertexType> global_layout;
    bool added_meshes = false;

    AddedMesh add_mesh(const Mesh& mesh)
    {
        added_meshes = true;
        auto compiled = compile_mesh(this, &cache, *vfs, mesh);
        return {geoms.add(std::move(compiled.geom)), materials.add(std::move(compiled.material))};
    }

    MaterialId add_global_shader(const Material& path)
    {
        ASSERT(added_meshes == false);
        auto material = compile_material(this, &cache, *vfs, path);
        const auto& shader = get_compiled_material_shader(cache, material.shader);
        global_layout = shader.mesh_layout.get_base_layout();
        return materials.add(std::move(material));
    }

    MaterialId duplicate_material(MaterialId id)
    {
        CompiledMaterial copy = materials[id];
        return materials.add(std::move(copy));
    }

    CompiledMaterial& get_material_ref(MaterialId id)
    {
        return materials[id];
    }
    
    
    RenderList render;
};

CompiledVertexTypeList get_compile_attribute_layouts(Engine* engine, const ShaderVertexAttributes& layout)
{
    return compile_attribute_layouts(engine->global_layout, {layout});
}

enum class ScopedRendererState
{
    adding, complete
};

struct ScopedRenderer
{
    Engine* engine;
    ScopedRendererState render_state = ScopedRendererState::adding;

    ScopedRenderer() = default;
    
    ScopedRenderer(const ScopedRenderer&) = delete;
    ScopedRenderer(ScopedRenderer&&) = delete;
    void operator=(const ScopedRenderer&) = delete;
    void operator=(ScopedRenderer&&) = delete;

    void add_directional_light(const DirectionalLight& d)
    {
        ASSERT(render_state == ScopedRendererState::adding);
        engine->render.add_directional_light(d);
    }

    void add_point_light(const PointLight& p)
    {
        ASSERT(render_state == ScopedRendererState::adding);
        engine->render.add_point_light(p);
    }

    void add_spot_light(const SpotLight& s)
    {
        ASSERT(render_state == ScopedRendererState::adding);
        engine->render.add_spot_light(s);
    }
    
    void add_mesh(GeomId geom_id, MaterialId material_id, const glm::mat4& model)
    {
        ASSERT(render_state == ScopedRendererState::adding);
        engine->render.add_mesh(geom_id, material_id, model);
    }

    void render_all()
    {
        ASSERT(render_state == ScopedRendererState::adding);
        render_state = ScopedRendererState::complete;

        engine->render.end();

        glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        for(const auto& c: engine->render.commands)
        {
            render_geom_with_material
            (
                engine->geoms[c.geom_id],
                engine->materials[c.material_id],
                engine->cache,
                {
                    engine->render.camera_position,
                    engine->render.projection_view,
                    c.model
                },
                engine->render.lights,
                &engine->render.light_status
            );
        }
    }

    ~ScopedRenderer()
    {
        ASSERT(render_state == ScopedRendererState::complete);
    }
};

ScopedRenderer create_render_list_for_perspective(Engine* engine, float aspect_ratio, const Camera& camera, std::optional<MaterialId> global_shader)
{
    engine->render.begin_perspective(aspect_ratio, camera, global_shader);
    return {engine};
}


// todo(Gustav): refactor to get lp as a argument isntead of engine?
const LightParams& get_light_params(const Engine& engine)
{
    return engine.lp;
}

} // namespace rendering


constexpr auto diffuse_color = HashedStringView{"Diffuse color"};
constexpr auto diffuse_texture = HashedStringView{"Diffuse texture"};
constexpr auto specular_texture = HashedStringView{"Specular texture"};
constexpr auto shininess_prop = HashedStringView{"Shininess"};
constexpr auto specular_strength_prop = HashedStringView{"Specular strength"};
constexpr auto tint_prop = HashedStringView{"Tint color"};


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
                .with_vec4(diffuse_color, "uColor", glm::vec4{white3, 1.0f})
                .with_vec3(tint_prop, "uMaterial.tint", white3)
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
                .with_vec3(diffuse_color, "uColor", glm::vec3{1.0f, 1.0f, 1.0f})
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
        if(path == "white.png")
        {
            return load_image_from_color
            (
                0xFFFFFFFF,
                TextureEdge::repeat,
                TextureRenderStyle::smooth,
                Transparency::exclude
            );
        }
        if(path == "no-specular.png")
        {
            return load_image_from_color
            (
                0x000000FF,
                TextureEdge::repeat,
                TextureRenderStyle::smooth,
                Transparency::exclude
            );
        }
        return std::nullopt;
    }
};


struct PointLightAndMaterial
{
    PointLight light;
    rendering::MaterialId material;
};

#if OLD_INPUT == 0

float two_button_input(bool p, bool n)
{
    if(p && n) { return 0.0f;}
    if(p) { return 1.0f; }
    if(n) { return -1.0f; }
    return 0.0f;
}

struct DemoInput : input::InputSystemBase
{
    // if imgui is enabled, we shouldn't capture mouse if imgui is currently using it
    explicit DemoInput(bool imgui_is_enabled) : imgui_enabled(imgui_is_enabled) {}
    bool imgui_enabled;

    // "internal" state
    bool is_mouse_captured = false;

    // input
    bool move_left = false;
    bool move_right = false;
    bool move_up = false;
    bool move_down = false;
    bool move_fwd = false;
    bool move_back = false;
    bool should_quit = false;
    float mouse_dx = 0.0f;
    float mouse_dy = 0.0f;

    float get_inout() const { return two_button_input(move_fwd, move_back); }
    float get_leftright() const { return two_button_input(move_right, move_left); }
    float get_updown() const { return two_button_input(move_up, move_down); }

    static constexpr float sensitivity = 0.1f;
    float get_look_leftright() const { return mouse_dx * sensitivity; }
    float get_look_updown() const { return mouse_dy * sensitivity; }

    void update_player_connections(input::unit_discovery, input::Platform*) const override
    {
        // there is always one connection
    }

    bool is_mouse_connected() const override
    {
        return is_mouse_captured;
    }

    void on_keyboard_key(input::KeyboardKey key, bool down) override
    {
        if(is_mouse_captured == false)
        {
            if(down == false && key == input::KeyboardKey::escape)
            {
                should_quit = true;
            }

            // mouse is not captured - don't do fps controller
            move_left = false;
            move_right = false;
            move_up = false;
            move_down = false;
            move_fwd = false;
            move_back = false;
            return;
        }
        
        switch(key)
        {
        case input::KeyboardKey::a: move_left = down; break;
        case input::KeyboardKey::d: move_right = down; break;
        case input::KeyboardKey::w: move_fwd = down; break;
        case input::KeyboardKey::s: move_back = down; break;
        case input::KeyboardKey::space: move_up = down; break;
        case input::KeyboardKey::ctrl_left: move_down = down; break;
        default: break;
        }

        if(down == false && key == input::KeyboardKey::escape)
        {
            is_mouse_captured = false;
        }
    }

    void on_mouse_axis(input::Axis2 axis, float relative_state, float /*absolute_state*/) override
    {
        if(is_mouse_captured == false )
        {
            mouse_dx = 0.0f;
            mouse_dy = 0.0f;
            return;
        }

        switch(axis)
        {
            case input::Axis2::x: mouse_dx = relative_state; break;
            case input::Axis2::y: mouse_dy = relative_state; break;
            default: break;
        }
    }

    void on_mouse_wheel(input::Axis2, float) override
    {
        // don't care about mouse wheel
    }

    void on_mouse_button(input::MouseButton button, bool down) override
    {
        if(down) { return; }
        if(button != input::MouseButton::left) {return;}

        if(imgui_enabled && ImGui::GetIO().WantCaptureMouse)
        {
            return;
        }

        is_mouse_captured = true;
    }

    // don't care about neither joystick nor gamepad
    void on_joystick_ball(input::JoystickId, input::Axis2, int, float) override {}
    void on_joystick_hat(input::JoystickId, input::Axis2, int, float) override {}
    void on_joystick_button(input::JoystickId, int, bool) override {}
    void on_joystick_axis(input::JoystickId, int, float) override {}
    void on_joystick_lost(input::JoystickId) override {}
    void on_gamecontroller_button(input::JoystickId, input::GamecontrollerButton, float) override {}
    void on_gamecontroller_axis(input::JoystickId, input::GamecontrollerAxis, float) override {}
    void on_gamecontroller_lost(input::JoystickId) override {}
};
#endif


bool ui_attenuation(Attenuation* a)
{
    bool changed = false;
    changed = ImGui::DragFloat("Attenuation constant", &a->constant, 0.01f) || changed;
    changed = ImGui::DragFloat("Attenuation linear", &a->linear, 0.01f) || changed;
    changed = ImGui::DragFloat("Attenuation quadratic", &a->quadratic, 0.01f) || changed;
    return changed;
}


bool ui_directional(DirectionalLight* light)
{
    bool changed = false;
    changed = ImGui::DragFloat("Strength", &light->ambient_strength, 0.01f) || changed;
    changed = ImGui::ColorEdit3("Ambient", glm::value_ptr(light->ambient)) || changed;
    changed = ImGui::ColorEdit3("Diffuse", glm::value_ptr(light->diffuse)) || changed;
    changed = ImGui::ColorEdit3("Specular", glm::value_ptr(light->specular)) || changed;
    changed = ImGui::DragFloat3("Position", glm::value_ptr(light->position), 0.01f) || changed;
    return changed;
}


bool ui_point(PointLight* light)
{
    bool changed = false;
    changed = ui_attenuation(&light->attenuation) || changed;
    changed = ImGui::DragFloat("Strength", &light->ambient_strength, 0.01f) || changed;
    changed = ImGui::ColorEdit3("Ambient", glm::value_ptr(light->ambient)) || changed;
    changed = ImGui::ColorEdit3("Diffuse", glm::value_ptr(light->diffuse)) || changed;
    changed = ImGui::ColorEdit3("Specular", glm::value_ptr(light->specular)) || changed;
    changed = ImGui::DragFloat3("Position", glm::value_ptr(light->position), 0.01f) || changed;
    return changed;
}


bool ui_spot(SpotLight* light)
{
    bool changed = false;
    changed = ui_attenuation(&light->attenuation) || changed;
    changed = ImGui::DragFloat("Ambient strength", &light->ambient_strength, 0.01f) || changed;
    changed = ImGui::ColorEdit3("Ambient", glm::value_ptr(light->ambient)) || changed;
    changed = ImGui::ColorEdit3("Diffuse", glm::value_ptr(light->diffuse)) || changed;
    changed = ImGui::ColorEdit3("Specular", glm::value_ptr(light->specular)) || changed;
    changed = ImGui::DragFloat("Cutoff", &light->cutoff, 0.1f) || changed;
    changed = ImGui::DragFloat("Outer cutoff", &light->outer_cutoff, 0.1f) || changed;
    return changed;
}


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

#if OLD_INPUT == 1
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
    #endif

    auto sdl_input = DemoInput{true};

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

    auto vfs = FixedFileVfs{};
    auto engine = rendering::Engine
    {
        &vfs,
        {
            NUMBER_OF_DIRECTIONAL_LIGHTS,
            NUMBER_OF_POINT_LIGHTS,
            NUMBER_OF_SPOT_LIGHTS
        }
    };

    // todo(Gustav): add override shaders so we can render just white polygon/points
    const auto white_only = engine.add_global_shader(rendering::Material{"unlit.glsl"}); // reuse unlit for white-only shader as it per default is white
    bool use_white_only = false;

    const auto crate = engine.add_mesh
    ({
        rendering::Material{"default.glsl"}
            .with_texture(diffuse_texture, "container_diffuse.png")
            .with_texture(specular_texture, "container_specular.png")
            ,
        create_box_mesh(1.0f)
    });
    const auto light = engine.add_mesh
    ({
        rendering::Material{"unlit.glsl"},
        create_box_mesh(0.2f)
    });
    const auto plane = engine.add_mesh
    ({
        rendering::Material{"default.glsl"}
            .with_texture(diffuse_texture, "container_diffuse.png")
            .with_texture(specular_texture, "container_specular.png")
            ,
        create_plane_mesh(plane_size, plane_size)
    });

    auto spot_light = ::SpotLight{};
    auto directional_light = ::DirectionalLight{};
    auto point_lights = std::array<PointLightAndMaterial, NUMBER_OF_POINT_LIGHTS>
    {
        PointLightAndMaterial{glm::vec3{ 0.7f,  0.2f,  2.0f}, light.material},
        PointLightAndMaterial{glm::vec3{ 2.3f, -3.3f, -4.0f}, engine.duplicate_material(light.material)},
        PointLightAndMaterial{glm::vec3{-4.0f,  2.0f, -12.0f}, engine.duplicate_material(light.material)},
        PointLightAndMaterial{glm::vec3{ 0.0f,  0.0f, -3.0f}, engine.duplicate_material(light.material)}
    };
    auto match_diffuse_color_for_point_light = [&](PointLightAndMaterial& pl)
    {
        // todo(Gustav): move function to engine?
        auto& material = engine.get_material_ref(pl.material);
        material.set_vec3_by_lookup(engine.cache, diffuse_color, pl.light.diffuse);
    };
    for(auto& pl: point_lights)
    {
        match_diffuse_color_for_point_light(pl);
    }

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
                auto renderer = rendering::create_render_list_for_perspective(&engine, aspect_ratio, camera, use_white_only ? std::make_optional(white_only) : std::nullopt);

                // render flying crates
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
                    renderer.add_mesh(crate.geom, crate.material, model);
                }

                // render lights
                renderer.add_directional_light(directional_light);
                renderer.add_spot_light(spot_light);
                for(const auto& pl: point_lights)
                {
                    renderer.add_point_light(pl.light);

                    // light_shader.set_vec3(uni_light_color, pl.diffuse);
                    const auto model = glm::translate(glm::mat4(1.0f), pl.light.position);
                    renderer.add_mesh(light.geom, pl.material, model);
                }
                
                // render ground
                {
                    const auto model = glm::translate(glm::mat4(1.0f), {0.0f, -3.5f, 0.0f});
                    renderer.add_mesh(plane.geom, plane.material, model);
                }

                renderer.render_all();
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
    const auto set_rendering_mode = [&rendering_mode, &use_white_only]()
    {
        switch(rendering_mode)
        {
            case 0: glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); use_white_only = false; break;
            case 1: glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); use_white_only = true; break;
            case 2: glPolygonMode(GL_FRONT_AND_BACK, GL_POINT); use_white_only = true; break;
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
                            if(ui_point(&point_lights[i].light))
                            {
                                match_diffuse_color_for_point_light(point_lights[i]);
                            }
                            ImGui::PopID();
                        }
                    }
                }

                if (ImGui::CollapsingHeader("Cubes"))
                {
                    // todo(Gustav): introduce again
                    #if 0
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

    #if OLD_INPUT == 1
    auto player = sdl_input.add_player();
    auto get = [](const input::Table& table, std::string_view name, float d=0.0f) -> float
    {
        const auto found = table.data.find(std::string{name});
        if(found == table.data.end()) { return d; }
        return found->second;
    };
    #endif

    return main_loop(input::unit_discovery::find_highest, std::move(windows), &sdl_input, [&](float dt) -> bool
    {
        #if OLD_INPUT == 1
        input::Table table;
        sdl_input.update_table(player, &table, dt);
        #endif

        if(animate)
        {
            time += dt;

            if(time > 2*pi)
            {
                time -= 2*pi;
            }
        }

        const bool should_quit
        #if OLD_INPUT == 1
        = get(table, quit, 1.0f) > 0.5f
        #else
        = sdl_input.should_quit
        #endif
        ;

        if(should_quit)
        {
            running = false;
        }

        const auto v = camera.create_vectors();

        #if OLD_INPUT == 1
        const auto input_inout = get(table, inout);
        const auto input_leftright = get(table, leftright);
        const auto input_updown = get(table, updown);
        const auto input_look_leftright = get(table, look_leftright);
        const auto input_look_updown = get(table, look_updown);
        #else
        const auto input_inout = sdl_input.get_inout();
        const auto input_leftright = sdl_input.get_leftright();
        const auto input_updown = sdl_input.get_updown();
        const auto input_look_leftright = sdl_input.get_look_leftright();
        const auto input_look_updown = sdl_input.get_look_updown();
        #endif

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
        camera.yaw += input_look_leftright * sensitivity;
        camera.pitch += input_look_updown * sensitivity;
        if(camera.pitch >  89.0f) camera.pitch =  89.0f;
        if(camera.pitch < -89.0f) camera.pitch = -89.0f;

        return running;
    });
}

