#include "tred/shader.h"

#include "glm/gtc/type_ptr.hpp"

#include "tred/opengl.h"
#include "tred/shader.util.h"
#include "tred/log.h"
#include "tred/cint.h"


bool
CheckShaderCompilationError(const char* name, unsigned int shader)
{
    int  success = 0;
    char log[512] = {0,};
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if(!success)
    {
        glGetShaderInfoLog(shader, 512, NULL, log);
        LOG_ERROR("ERROR: {} shader compilation failed\n{}\n", name, log);
        return false;
    }

    return true;
}


bool
CheckShaderLinkError(unsigned int program)
{
    int  success = 0;
    char log[512] = {0,};
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(program, 512, NULL, log);
        LOG_ERROR("ERROR: shader linking failed\n{}\n", log);
        return false;
    }

    return true;
}


void
UploadShaderSource(unsigned int shader, std::string_view source)
{
    const char* const s = &source[0];
    const int length = Csizet_to_int(source.length());
    glShaderSource(shader, 1, &s, &length);
};



void
BindShaderAttributeLocation(unsigned int shader_program, const CompiledVertexLayout& layout)
{
    for(const auto& b: layout.elements)
    {
        glBindAttribLocation(shader_program, Cint_to_gluint(b.index), b.name.c_str());
    }
}


void
VerifyShaderAttributeLocation(unsigned int shader_program, const CompiledVertexLayout& layout)
{
    for(const auto& b: layout.elements)
    {
        const auto actual_index = glGetAttribLocation
        (
            shader_program,
            b.name.c_str()
        );

        if(actual_index != b.index)
        {
            LOG_ERROR
            (
                "ERROR: {} was bound to {} but requested at {}",
                b.name.c_str(),
                actual_index,
                b.index
            );
        }
    }
}


Shader::Shader
(
    std::string_view vertex_source,
    std::string_view fragment_source,
    const CompiledVertexLayout& layout
)
    : shader_program(glCreateProgram())
    , vertex_types(layout.types)
{
    const auto vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    UploadShaderSource(vertex_shader, vertex_source);
    glCompileShader(vertex_shader);
    const auto vertex_ok = CheckShaderCompilationError("vertex", vertex_shader);

    const auto fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    UploadShaderSource(fragment_shader, fragment_source);
    glCompileShader(fragment_shader);
    const auto fragment_ok = CheckShaderCompilationError("fragment", fragment_shader);

    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    BindShaderAttributeLocation(shader_program, layout);
    glLinkProgram(shader_program);
    const auto link_ok = CheckShaderLinkError(shader_program);

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    ClearShaderProgram();

    if(vertex_ok && fragment_ok && link_ok)
    {
        // nop
        VerifyShaderAttributeLocation(shader_program, layout);
    }
    else
    {
        Delete();
    }
}


void
Shader::Use() const
{
    SetShaderProgram(shader_program, vertex_types);
}


void
Shader::Delete()
{
    ClearShaderProgram();
    glDeleteProgram(shader_program);
    shader_program = 0;
}


Uniform
Shader::GetUniform(const std::string& name) const
{
    const auto uni = Uniform{name, glGetUniformLocation(shader_program, name.c_str()), shader_program};
    if(uni.IsValid() == false)
    {
        LOG_ERROR("Uniform {} not found", name.c_str());
    }
    return uni;
}


// shader neeeds to be bound
void
Shader::SetFloat(const Uniform& uniform, float value) const
{
    assert(IsShaderBound(shader_program));
    if(uniform.IsValid() == false) { return; }
    assert(uniform.debug_shader_program == shader_program);

    assert(uniform.texture == -1 && "uniform is a texture not a float");
    glUniform1f(uniform.location, value);
}

void
Shader::SetVec3(const Uniform& uniform, float x, float y, float z)
{
    assert(IsShaderBound(shader_program));
    if(uniform.IsValid() == false) { return; }
    assert(uniform.debug_shader_program == shader_program);

    assert(uniform.texture == -1 && "uniform is a texture not a vec3");
    glUniform3f(uniform.location, x, y, z);
}


void
Shader::SetVec3(const Uniform& uniform, const glm::vec3& v)
{
    SetVec3(uniform, v.x, v.y, v.z);
}


void
Shader::SetVec4(const Uniform& uniform, float x, float y, float z, float w)
{
    assert(IsShaderBound(shader_program));
    if(uniform.IsValid() == false) { return; }
    assert(uniform.debug_shader_program == shader_program);

    assert(uniform.texture == -1 && "uniform is a texture not a vec4");
    glUniform4f(uniform.location, x, y, z, w);
}

void
Shader::SetVec4(const Uniform& uniform, const glm::vec4& v)
{
    SetVec4(uniform, v.x, v.y, v.z, v.w);
}


void
Shader::SetTexture(const Uniform& uniform)
{
    assert(IsShaderBound(shader_program));
    if(uniform.IsValid() == false) { return; }
    assert(uniform.debug_shader_program == shader_program);

    assert(uniform.texture >= 0 && "uniform needs to be a texture");
    glUniform1i(uniform.location, uniform.texture);
}


void
Shader::SetMat(const Uniform& uniform, const glm::mat4& mat)
{
    assert(IsShaderBound(shader_program));
    if(uniform.IsValid() == false) { return; }
    assert(uniform.debug_shader_program == shader_program);

    assert(uniform.texture == -1 && "uniform is a texture not a matrix4");
    glUniformMatrix4fv(uniform.location, 1, GL_FALSE, glm::value_ptr(mat));
}


void
Shader::SetMat(const Uniform& uniform, const glm::mat3& mat)
{
    assert(IsShaderBound(shader_program));
    if(uniform.IsValid() == false) { return; }
    assert(uniform.debug_shader_program == shader_program);

    assert(uniform.texture == -1 && "uniform is a texture not a matrix3");
    glUniformMatrix3fv(uniform.location, 1, GL_FALSE, glm::value_ptr(mat));
}


void
SetupTextures(Shader* shader, std::vector<Uniform*> uniform_list)
{
    // OpenGL should support atleast 16 textures
    assert(uniform_list.size() <= 16);

    shader->Use();

    int index = 0;
    for(const auto& uniform: uniform_list)
    {
        uniform->texture = index;
        index += 1;
        shader->SetTexture(*uniform);
    }
}

