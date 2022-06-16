#include "tred/render/opengl_state.h"

#include "tred/assert.h"

#include "tred/dependency_opengl.h"


template<typename T, typename TChange>
void apply_generic_state(std::optional<T>* current_state, std::optional<T> new_state, TChange change_function)
{
    if
    (
        new_state.has_value() == false
        ||
        (current_state->has_value() && **current_state == *new_state)
    )
    {
        return;
    }

    ASSERT(new_state.has_value());

    change_function(*new_state);
    *current_state = new_state;
}

void apply_state(std::optional<bool>* current_state, std::optional<bool> new_state, GLenum gl_type)
{
    apply_generic_state<bool>
    (
        current_state, new_state,
        [gl_type](bool enable)
        {
            if(enable)
            {
                glEnable(gl_type);
            }
            else
            {
                glDisable(gl_type);
            }
        }
    );
}

void apply(OpenglStates* current_states, const OpenglStates& new_states)
{
    #define APPLY_STATE(name, gl) apply_state(&current_states->name, new_states.name, gl)
    APPLY_STATE(cull_face, GL_CULL_FACE);
    APPLY_STATE(blending, GL_BLEND);
    APPLY_STATE(depth_test, GL_DEPTH_TEST);
    #undef APPLY_STATE
}
