#include "tred/shader.util.h"

#include "tred/opengl.h"


namespace
{
    vertex_types debug_current_shader_types;
    unsigned int debug_current_shader_program = 0;
}


void
set_shader_program(unsigned int new_program, const vertex_types& types)
{
    debug_current_shader_program = new_program;
    debug_current_shader_types = types;
    glUseProgram(new_program);
}


bool
is_bound_for_shader(const vertex_types& debug_shader_types)
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
