#version 330 core

in vec4 fColor;
in vec2 fTexCoord;

out vec4 FragColor;

uniform sampler2D uTexture;
uniform sampler2D uAwesome;

void main()
{
    vec4 texture_sample = texture(uTexture, fTexCoord);
    vec4 awesome_sample = texture(uAwesome, fTexCoord);
    FragColor = fColor * mix(texture_sample, awesome_sample, 0.5);
}
