attribute position3 aPos;




#shader vertex
#version 330 core

in vec3 aPos;

// u for uniform
uniform mat4 uTransform;

void main()
{
    gl_Position = uTransform * vec4(aPos, 1.0);
}




#shader fragment
#version 330 core

out vec4 FragColor;

uniform vec3 uColor;

void main()
{
    FragColor = vec4(uColor, 1);
}
