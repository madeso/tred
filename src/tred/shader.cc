#include "tred/shader.h"

#include "tred/compiler_warning.h"

DISABLE_WARNING_PUSH
DISABLE_WARNING_NONSTANDARD_NAMELESS_STRUCT_UNION
#include "glm/gtc/type_ptr.hpp"
DISABLE_WARNING_POP

#include "tred/opengl.h"
#include "tred/shader.util.h"
#include "tred/log.h"
#include "tred/cint.h"


bool
check_shader_compilation_error(const char* name, unsigned int shader)
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
check_shader_link_error(unsigned int program)
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
upload_shader_source(unsigned int shader, std::string_view source)
{
    const char* const s = &source[0];
    const int length = Csizet_to_int(source.length());
    glShaderSource(shader, 1, &s, &length);
};



void
bind_shader_attribute_location(unsigned int shader_program, const CompiledVertexLayout& layout)
{
    for(const auto& b: layout.elements)
    {
        glBindAttribLocation(shader_program, Cint_to_gluint(b.index), b.name.c_str());
    }
}


void
verify_shader_attribute_location(unsigned int shader_program, const CompiledVertexLayout& layout)
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
    upload_shader_source(vertex_shader, vertex_source);
    glCompileShader(vertex_shader);
    const auto vertex_ok = check_shader_compilation_error("vertex", vertex_shader);

    const auto fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    upload_shader_source(fragment_shader, fragment_source);
    glCompileShader(fragment_shader);
    const auto fragment_ok = check_shader_compilation_error("fragment", fragment_shader);

    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    bind_shader_attribute_location(shader_program, layout);
    glLinkProgram(shader_program);
    const auto link_ok = check_shader_link_error(shader_program);

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    clear_shader_program();

    if(vertex_ok && fragment_ok && link_ok)
    {
        // nop
        verify_shader_attribute_location(shader_program, layout);
    }
    else
    {
        cleanup();
    }
}


void
Shader::use() const
{
    set_shader_program(shader_program, vertex_types);
}


void
Shader::cleanup()
{
    clear_shader_program();
    glDeleteProgram(shader_program);
    shader_program = 0;
}


Uniform
Shader::get_uniform(const std::string& name) const
{
    const auto uni = Uniform{name, glGetUniformLocation(shader_program, name.c_str()), shader_program};
    if(uni.is_valid() == false)
    {
        LOG_ERROR("Uniform {} not found", name.c_str());
    }
    return uni;
}


// shader neeeds to be bound
void
Shader::set_float(const Uniform& uniform, float value) const
{
    assert(is_shader_bound(shader_program));
    if(uniform.is_valid() == false) { return; }
    assert(uniform.debug_shader_program == shader_program);

    assert(uniform.texture == -1 && "uniform is a texture not a float");
    glUniform1f(uniform.location, value);
}

void
Shader::set_vec3(const Uniform& uniform, float x, float y, float z)
{
    assert(is_shader_bound(shader_program));
    if(uniform.is_valid() == false) { return; }
    assert(uniform.debug_shader_program == shader_program);

    assert(uniform.texture == -1 && "uniform is a texture not a vec3");
    glUniform3f(uniform.location, x, y, z);
}


void
Shader::set_vec3(const Uniform& uniform, const glm::vec3& v)
{
    set_vec3(uniform, v.x, v.y, v.z);
}


void
Shader::set_vec4(const Uniform& uniform, float x, float y, float z, float w)
{
    assert(is_shader_bound(shader_program));
    if(uniform.is_valid() == false) { return; }
    assert(uniform.debug_shader_program == shader_program);

    assert(uniform.texture == -1 && "uniform is a texture not a vec4");
    glUniform4f(uniform.location, x, y, z, w);
}

void
Shader::set_vec4(const Uniform& uniform, const glm::vec4& v)
{
    set_vec4(uniform, v.x, v.y, v.z, v.w);
}


void
Shader::set_texture(const Uniform& uniform)
{
    assert(is_shader_bound(shader_program));
    if(uniform.is_valid() == false) { return; }
    assert(uniform.debug_shader_program == shader_program);

    assert(uniform.texture >= 0 && "uniform needs to be a texture");
    glUniform1i(uniform.location, uniform.texture);
}


void
Shader::set_mat(const Uniform& uniform, const glm::mat4& mat)
{
    assert(is_shader_bound(shader_program));
    if(uniform.is_valid() == false) { return; }
    assert(uniform.debug_shader_program == shader_program);

    assert(uniform.texture == -1 && "uniform is a texture not a matrix4");
    glUniformMatrix4fv(uniform.location, 1, GL_FALSE, glm::value_ptr(mat));
}


void
Shader::set_mat(const Uniform& uniform, const glm::mat3& mat)
{
    assert(is_shader_bound(shader_program));
    if(uniform.is_valid() == false) { return; }
    assert(uniform.debug_shader_program == shader_program);

    assert(uniform.texture == -1 && "uniform is a texture not a matrix3");
    glUniformMatrix3fv(uniform.location, 1, GL_FALSE, glm::value_ptr(mat));
}


void
setup_textures(Shader* shader, std::vector<Uniform*> uniform_list)
{
    // OpenGL should support atleast 16 textures
    assert(uniform_list.size() <= 16);

    shader->use();

    int index = 0;
    for(const auto& uniform: uniform_list)
    {
        uniform->texture = index;
        index += 1;
        shader->set_texture(*uniform);
    }
}

