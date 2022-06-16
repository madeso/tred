#include "tred/render/opengl_utils.h"

#include "tred/dependency_opengl.h"
#include "tred/render/opengl_debug.h"
#include "tred/render/opengl_state.h"

void opengl_setup(OpenglStates* state)
{
    setup_opengl_debug();

    apply
    (
        state,
        OpenglStates{}
            .set_cull_face(true)
    );
    
    glCullFace(GL_BACK);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


void opengl_set2d(OpenglStates* states)
{
    apply
    (
        states,
        OpenglStates{}
            .set_depth_test(false)
            .set_blending(true)
    );
}


void opengl_set3d(OpenglStates* states)
{
    apply
    (
        states,
        OpenglStates{}
            .set_depth_test(true)
            .set_blending(false)
    );
}

