#version 330 core

struct Material
{
    vec3 tint;

    sampler2D diffuse;
    sampler2D specular;

    float shininess;
    float specular_strength;
};

struct DirectionalLight
{
    vec3 normalized_direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 fNormal;
in vec3 fFragmentPosition;
in vec4 fColor;
in vec2 fTexCoord;

out vec4 FragColor;

uniform Material uMaterial;

uniform DirectionalLight uDirectionalLight;
uniform vec3 uViewPosition;


vec3
CalculateBaseLight
(
    vec3 light_ambient,
    vec3 light_diffuse,
    vec3 light_specular,
    vec3 object_color,
    vec3 specular_sample,
    vec3 normal,
    vec3 view_direction,
    vec3 light_direction
)
{
    vec3 reflection_direction = reflect(-light_direction, normal);
    float diffuse_component =      max(0.0f, dot(normal, light_direction));
    float specular_component = pow(max(0.0f, dot(view_direction, reflection_direction)), uMaterial.shininess);

    vec3 ambient = object_color * light_ambient * uMaterial.tint;
    vec3 diffuse = object_color * light_diffuse * uMaterial.tint * diffuse_component;
    vec3 specular = light_specular * specular_sample * uMaterial.specular_strength * specular_component;

    return ambient + diffuse + specular;
}


vec3
CalculateDirectionalLight
(
    DirectionalLight light,
    vec3 object_color,
    vec3 specular_sample,
    vec3 normal,
    vec3 view_direction
)
{
    vec3 light_direction = -light.normalized_direction;
    return CalculateBaseLight
    (
        light.ambient, light.diffuse, light.specular,
        object_color, specular_sample, normal, view_direction, light_direction
    );
}


void
main()
{
    vec4 diffuse_sample = texture(uMaterial.diffuse, fTexCoord);
    vec3 specular_sample = texture(uMaterial.specular, fTexCoord).rgb;
    vec3 object_color = fColor.rgb * diffuse_sample.rgb;

    vec3 normal = normalize(fNormal);
    // vec3 light_direction = normalize(uLight.position - fFragmentPosition);
    vec3 view_direction = normalize(uViewPosition - fFragmentPosition);
    
    FragColor = vec4(CalculateDirectionalLight(uDirectionalLight, object_color, specular_sample, normal, view_direction), 1.0f);
}
