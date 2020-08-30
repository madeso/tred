#version 330 core

in vec4 fColor;
in vec2 fTexCoord;

out vec4 FragColor;

uniform sampler2D uTexture;
uniform sampler2D uDecal;
uniform vec3 uLightColor;

void main()
{
    vec4 texture_sample = texture(uTexture, fTexCoord);
    vec4 decal_sample = texture(uDecal, fTexCoord);
    FragColor = vec4(uLightColor, 1.0f) * fColor * mix(texture_sample, vec4(decal_sample.rgb, 1), decal_sample.a);
}
