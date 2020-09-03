#version 330 core

in vec3 fNormal;
in vec3 fFragmentPosition;
in vec4 fColor;
in vec2 fTexCoord;

out vec4 FragColor;

uniform sampler2D uTexture;
uniform sampler2D uDecal;

uniform float uLightAmbientStrength;
uniform float uSpecularStrength;
uniform float uShininess;
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
    float specular_component = pow(max(dot(view_direction, reflection_direction), 0.0), uShininess);
    
    vec3 ambient = uLightColor * uLightAmbientStrength;
    vec3 diffuse = uLightColor * max(0.0, dot(normal, light_direction));
    vec3 specular = uLightColor * uSpecularStrength * specular_component;

    FragColor = vec4((ambient + diffuse + specular) * object_color, 1.0f);
}
