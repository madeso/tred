#include "tred/render_func.h"

#include "tred/dependency_glad.h"


void clear_stencil_and_depth()
{
    glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}


void set_poly_mode_to_fill()
{
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}


void set_poly_mode_to_line()
{
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}


void set_poly_mode_to_point()
{
    glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
}

