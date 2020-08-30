#version 330 core

in vec4 fColor;
in vec2 fTexCoord;

out vec4 FragColor;

uniform sampler2D uTexture;
uniform sampler2D uDecal;

uniform float uLightAmbientStrength;
uniform vec3 uLightColor;

void main()
{
    vec4 texture_sample = texture(uTexture, fTexCoord);
    vec4 decal_sample = texture(uDecal, fTexCoord);
    vec3 object_color = fColor.rgb * mix(texture_sample.rgb, decal_sample.rgb, decal_sample.a);
    
    vec3 ambient = uLightColor * uLightAmbientStrength;

    FragColor = vec4(ambient * object_color, 1.0f);
}
