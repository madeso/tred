#version 330 core

in vec3 fNormal;
in vec3 fFragmentPosition;
in vec4 fColor;
in vec2 fTexCoord;

out vec4 FragColor;

uniform sampler2D uTexture;
uniform sampler2D uDecal;

uniform float uLightAmbientStrength;
uniform vec3 uLightColor;
uniform vec3 uLightPosition;

void main()
{
    vec4 texture_sample = texture(uTexture, fTexCoord);
    vec4 decal_sample = texture(uDecal, fTexCoord);
    vec3 object_color = fColor.rgb * mix(texture_sample.rgb, decal_sample.rgb, decal_sample.a);

    vec3 normal = normalize(fNormal);
    vec3 light_direction = normalize(uLightPosition - fFragmentPosition);
    
    vec3 ambient = uLightColor * uLightAmbientStrength;
    vec3 diffuse = uLightColor * max(0.0, dot(normal, light_direction));

    FragColor = vec4((ambient + diffuse) * object_color, 1.0f);
}
