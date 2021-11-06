#include "tred/shader.util.h"

#include "tred/dependency_opengl.h"


namespace
{
    VertexTypes debug_current_shader_types;
    unsigned int debug_current_shader_program = 0;
}


void
set_shader_program(unsigned int new_program, const VertexTypes& types)
{
    debug_current_shader_program = new_program;
    debug_current_shader_types = types;
    glUseProgram(new_program);
}


bool
is_bound_for_shader(const VertexTypes& debug_shader_types)
{
    return debug_shader_types == debug_current_shader_types;
}

bool
is_shader_bound(unsigned int program)
{
    return debug_current_shader_program == program;
}


void
clear_shader_program()
{
    set_shader_program(0, {});
}
