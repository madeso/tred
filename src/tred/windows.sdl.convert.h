#pragma once

#include "glm/glm.hpp"

#include "tred/dependency_sdl.h"
#include "tred/input/key.h"
#include "tred/input/axis.h"


namespace sdl
{
    glm::ivec2 get_hat_values(Uint8 hat);

    input::keyboard_key to_keyboard_key(SDL_Keysym key);

    input::mouse_button to_mouse_button(Uint8 mb);

    input::gamecontroller_button to_controller_button(SDL_GameControllerButton b);
    input::gamecontroller_button to_controller_button(SDL_GameControllerAxis b);
    input::gamecontroller_axis to_controller_axis(SDL_GameControllerAxis b);
}

