#version 330 core

struct Material
{
    vec3 tint;

    sampler2D diffuse;

    vec3 specular;
    float shininess;
};

struct Light
{
    vec3 position;

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

uniform Light uLight;
uniform vec3 uViewPosition;


void main()
{
    vec4 diffuse_sample = texture(uMaterial.diffuse, fTexCoord);
    vec3 object_color = fColor.rgb * diffuse_sample.rgb;

    vec3 normal = normalize(fNormal);
    vec3 light_direction = normalize(uLight.position - fFragmentPosition);
    vec3 view_direction = normalize(uViewPosition - fFragmentPosition);
    vec3 reflection_direction = reflect(-light_direction, normal);
    float diffuse_component =      max(0.0f, dot(normal, light_direction));
    float specular_component = pow(max(0.0f, dot(view_direction, reflection_direction)), uMaterial.shininess);
    
    vec3 ambient = uLight.ambient * uMaterial.tint;
    vec3 diffuse = uLight.diffuse * uMaterial.tint * diffuse_component;
    vec3 specular = uLight.specular * uMaterial.specular * specular_component;

    FragColor = vec4((ambient + diffuse + specular) * object_color, 1.0f);
}
