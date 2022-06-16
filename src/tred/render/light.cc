#include "tred/render/light.h"


DirectionalLight::DirectionalLight()
    : ambient_strength(0.1f)
    , ambient(white3)
    , diffuse(white3)
    , specular(white3)
{
}


DirectionalLight::DirectionalLight
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


DirectionalLight
DirectionalLight::create_no_light()
{
    return DirectionalLight{0.0f, black3, black3, black3};
}


// todo(Gustav): make a property instead, function only here for demo purposes
glm::vec3
DirectionalLight::GetDirection() const
{
    return glm::normalize(-position);
}


PointLight::PointLight(const glm::vec3& p)
    : position(p)
{
}
