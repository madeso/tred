#version 330 core

in vec3 aPos;

// u for uniform
uniform mat4 uTransform;

void main()
{
    gl_Position = uTransform * vec4(aPos, 1.0);
}
