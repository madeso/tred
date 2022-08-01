#include "tred/render/render_func.h"


namespace render
{

void clear_stencil_and_depth()
{
    glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}


}
