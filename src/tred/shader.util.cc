#include "tred/shader.util.h"

#include "tred/opengl.h"


namespace
{
    VertexTypes debug_current_shader_types;
    unsigned int debug_current_shader_program = 0;
}


void
SetShaderProgram(unsigned int new_program, const VertexTypes& types)
{
    debug_current_shader_program = new_program;
    debug_current_shader_types = types;
    glUseProgram(new_program);
}


bool
IsBoundForShader(const VertexTypes& debug_shader_types)
{
    return debug_shader_types == debug_current_shader_types;
}

bool
IsShaderBound(unsigned int program)
{
    return debug_current_shader_program == program;
}


void
ClearShaderProgram()
{
    SetShaderProgram(0, {});
}
