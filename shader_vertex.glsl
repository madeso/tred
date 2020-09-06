#version 330 core

// a for attribute
in vec3 aPos;
in vec3 aNormal;
in vec4 aColor;
in vec2 aTexCoord;

// u for uniform
uniform vec4 uColor;
uniform mat4 uTransform;
uniform mat4 uModelTransform;
uniform mat3 uNormalMatrix;

// f for fragment
out vec4 fColor;
out vec3 fFragmentPosition;
out vec3 fNormal;
out vec2 fTexCoord;


void
main()
{
    gl_Position = uTransform * vec4(aPos, 1.0);
    fFragmentPosition = vec3(uModelTransform * vec4(aPos, 1.0));
    fColor = aColor * uColor;
    fTexCoord = aTexCoord;
    fNormal = uNormalMatrix * aNormal;
}
