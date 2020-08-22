#version 330 core

// a for attribute
in vec3 aPos;
in vec4 aColor;
in vec2 aTexCoord;

// u for uniform
uniform vec4 uColor;

// f for fragment
out vec4 fColor;
out vec2 fTexCoord;

void main()
{
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
    fColor = aColor * uColor;
    fTexCoord = aTexCoord;
}
