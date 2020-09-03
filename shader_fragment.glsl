#version 330 core

struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

in vec3 fNormal;
in vec3 fFragmentPosition;
in vec4 fColor;
in vec2 fTexCoord;

out vec4 FragColor;

uniform sampler2D uTexture;
uniform sampler2D uDecal;

uniform Material uMaterial;

uniform vec3 uLightColor;
uniform vec3 uLightPosition;
uniform vec3 uViewPosition;


void main()
{
    vec4 texture_sample = texture(uTexture, fTexCoord);
    vec4 decal_sample = texture(uDecal, fTexCoord);
    vec3 object_color = fColor.rgb * mix(texture_sample.rgb, decal_sample.rgb, decal_sample.a);

    vec3 normal = normalize(fNormal);
    vec3 light_direction = normalize(uLightPosition - fFragmentPosition);
    vec3 view_direction = normalize(uViewPosition - fFragmentPosition);
    vec3 reflection_direction = reflect(-light_direction, normal);
    float diffuse_component = max(0.0, dot(normal, light_direction));
    float specular_component = pow(max(dot(view_direction, reflection_direction), 0.0), uMaterial.shininess);
    
    vec3 ambient = uLightColor * uMaterial.ambient;
    vec3 diffuse = uLightColor * uMaterial.diffuse * diffuse_component;
    vec3 specular = uLightColor * uMaterial.specular * specular_component;

    FragColor = vec4((ambient + diffuse + specular) * object_color, 1.0f);
}
