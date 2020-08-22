#version 330 core

in vec4 fColor;
in vec2 fTexCoord;

out vec4 FragColor;

uniform sampler2D uTexture;

void main()
{
    FragColor = fColor * texture(uTexture, fTexCoord);
}
