#include "tred/render/shader.util.h"

#include "tred/dependency_opengl.h"

namespace render
{

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
is_bound_for_shader(const std::unordered_set<VertexType>&  debug_mesh_shader_types)
{
    for(auto t: debug_current_shader_types)
    {
        if(debug_mesh_shader_types.find(t) == debug_mesh_shader_types.end())
        {
            // if shader type isn't found in mesh
            // then error out
            return false;
        }
    }
    return true;
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

}
