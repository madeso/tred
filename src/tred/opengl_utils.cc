#include "tred/opengl_utils.h"

#include "tred/dependency_opengl.h"
#include "tred/opengl_debug.h"


void opengl_setup()
{
    setup_opengl_debug();

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK); // remove back faces
}


void opengl_set2d()
{
    glDisable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


void opengl_set3d()
{
    glEnable(GL_DEPTH_TEST);

    glDisable(GL_BLEND);
}

