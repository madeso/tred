#version 330 core

// a for attribute
in vec3 aPos;
in vec4 aColor;

// f for fragment
out vec4 fColor;

void main()
{
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
    fColor = aColor;
}
