#include "tred/render/light.uniforms.h"

#include <cmath>

#include "tred/render/shader.h"
#include "tred/render/light.h"
#include "tred/render/light.active.h"
#include "tred/render/light.params.h"


namespace render
{


DirectionalLightUniforms::DirectionalLightUniforms
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
DirectionalLightUniforms::set_shader(const ShaderProgram& shader, const DirectionalLight& light) const
{
    shader.set_vec3(direction, light.GetDirection());
    shader.set_vec3(ambient, light.ambient * light.ambient_strength);
    shader.set_vec3(diffuse, light.diffuse);
    shader.set_vec3(specular, light.specular);
}


void DirectionalLightUniforms::turn_on_light(const ShaderProgram& shader, const DirectionalLight& light) const
{
    set_shader(shader, light);
}


void DirectionalLightUniforms::turn_off_light(const ShaderProgram& shader) const
{
    set_shader(shader, DirectionalLight::create_no_light());
}



AttenuationUniforms::AttenuationUniforms
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
AttenuationUniforms::set_shader(const ShaderProgram& shader, const Attenuation& att) const
{
    shader.set_float(constant, att.constant);
    shader.set_float(linear, att.linear);
    shader.set_float(quadratic, att.quadratic);
}


PointLightUniforms::PointLightUniforms
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
PointLightUniforms::set_shader(const ShaderProgram& shader, const PointLight& light) const
{
    attenuation.set_shader(shader, light.attenuation);
    shader.set_vec3(position, light.position);
    shader.set_vec3(ambient, light.ambient * light.ambient_strength);
    shader.set_vec3(diffuse, light.diffuse);
    shader.set_vec3(specular, light.specular);
}


void
PointLightUniforms::turn_on_light(const ShaderProgram& shader, const PointLight& light) const
{
    set_shader(shader, light);
}


void
PointLightUniforms::turn_off_light(const ShaderProgram& shader) const
{
    auto dark = PointLight{glm::vec3{0.0f, 0.0f, 0.0f}};

    dark.attenuation = zero_attenuation();
    dark.ambient_strength = 0.0f;

    dark.ambient = {0.0f, 0.0f, 0.0f};
    dark.diffuse = {0.0f, 0.0f, 0.0f};
    dark.specular = {0.0f, 0.0f, 0.0f};

    set_shader(shader, dark);
}



SpotLightUniforms::SpotLightUniforms
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
SpotLightUniforms::set_shader(const ShaderProgram& shader, const SpotLight& light) const
{
    attenuation.set_shader(shader, light.attenuation);
    shader.set_vec3(position, light.position);
    shader.set_vec3(direction, light.direction);
    shader.set_float(cos_cutoff, std::cos(glm::radians(light.cutoff)));
    shader.set_float(cos_outer_cutoff, std::cos(glm::radians(light.outer_cutoff)));
    shader.set_vec3(ambient, light.ambient * light.ambient_strength);
    shader.set_vec3(diffuse, light.diffuse);
    shader.set_vec3(specular, light.specular);
}



void
SpotLightUniforms::turn_on_light(const ShaderProgram& shader, const SpotLight& light) const
{
    set_shader(shader, light);
}



void
SpotLightUniforms::turn_off_light(const ShaderProgram& shader) const
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


LightUniforms::LightUniforms(const ShaderProgram& shader, const LightParams& lp)
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

void LightUniforms::set_shader(const ShaderProgram& prog, const LightData& data, LightStatus* ls) const
{
    apply_data(prog, data.directional_lights, directional_lights, &ls->applied_directional_lights);
    apply_data(prog, data.point_lights, point_lights, &ls->applied_point_lights);
    apply_data(prog, data.spot_lights, spot_lights, &ls->applied_spot_lights);
}


}
