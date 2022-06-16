#pragma once

#include "tred/colors.h"


struct DirectionalLight
{
    // todo(Gustav): remove this?
    glm::vec3 position = glm::vec3{0.0f, 0.0f, 0.0f};

    float ambient_strength;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    DirectionalLight();

    DirectionalLight
    (
        float as,
        const glm::vec3& a,
        const glm::vec3& d,
        const glm::vec3& s
    );

    DirectionalLight(const DirectionalLight&) = default;
    DirectionalLight& operator=(const DirectionalLight&) = default;

    static DirectionalLight create_no_light();

    // todo(Gustav): make a property instead, function only here for demo purposes
    glm::vec3
    GetDirection() const;
};


struct Attenuation
{
    float constant = 1.0f;
    float linear = 0.09f;
    float quadratic = 0.032f;
};

constexpr Attenuation zero_attenuation()
{
    return Attenuation{0.0f, 0.0f, 0.0f};
}


struct PointLight
{
    Attenuation attenuation;

    glm::vec3 position;

    float ambient_strength = 0.1f;
    glm::vec3 ambient =  glm::vec3{1.0f, 1.0f, 1.0f};
    glm::vec3 diffuse =  glm::vec3{1.0f, 1.0f, 1.0f};
    glm::vec3 specular = glm::vec3{1.0f, 1.0f, 1.0f};

    PointLight(const glm::vec3& p);
    PointLight(const PointLight&) = default;
    PointLight& operator=(const PointLight&) = default;
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

