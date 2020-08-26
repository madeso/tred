#version 330 core

// a for attribute
in vec3 aPos;
in vec4 aColor;
in vec2 aTexCoord;

// u for uniform
uniform vec4 uColor;
uniform mat4 uTransform;

// f for fragment
out vec4 fColor;
out vec2 fTexCoord;

void main()
{
    gl_Position = uTransform * vec4(aPos, 1.0);
    fColor = aColor * uColor;
    fTexCoord = aTexCoord;
}
