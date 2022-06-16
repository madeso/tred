#pragma once

#include "tred/render/uniform.h"

struct ShaderProgram;
struct Attenuation;
struct DirectionalLight;
struct PointLight;
struct SpotLight;


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
    );

    void
    set_shader(const ShaderProgram& shader, const DirectionalLight& light) const;

    void turn_on_light(const ShaderProgram& shader, const DirectionalLight& light) const;

    void turn_off_light(const ShaderProgram& shader) const;
};



struct AttenuationUniforms
{
    Uniform constant;
    Uniform linear;
    Uniform quadratic;

    AttenuationUniforms
    (
        const ShaderProgram& shader,
        const std::string& base_name
    );

    void
    set_shader(const ShaderProgram& shader, const Attenuation& att) const;
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
    );

    void
    set_shader(const ShaderProgram& shader, const PointLight& light) const;

    void
    turn_on_light(const ShaderProgram& shader, const PointLight& light) const;

    void
    turn_off_light(const ShaderProgram& shader) const;
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
    );

    void
    set_shader(const ShaderProgram& shader, const SpotLight& light) const;

    void
    turn_on_light(const ShaderProgram& shader, const SpotLight& light) const;

    void
    turn_off_light(const ShaderProgram& shader) const;
};


namespace render
{

struct LightParams;
struct LightData;
struct LightStatus;

struct LightUniforms
{
    std::vector<DirectionalLightUniforms> directional_lights;
    std::vector<PointLightUniforms> point_lights;
    std::vector<SpotLightUniforms> spot_lights;

    LightUniforms(const ShaderProgram& shader, const LightParams& lp);

    void set_shader(const ShaderProgram& prog, const LightData& data, LightStatus* ls) const;
};

}
