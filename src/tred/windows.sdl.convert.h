#pragma once

#include "SDL.h"
#include "glm/glm.hpp"

#include "tred/input/key.h"
#include "tred/input/axis.h"


namespace sdl
{
    glm::ivec2 GetHatValues(Uint8 hat);

    input::Key ToKey(SDL_Keysym key);

    input::MouseButton ToMouseButton(Uint8 mb);

    input::GamecontrollerButton ToButton(SDL_GameControllerButton b);
    input::GamecontrollerButton ToButton(SDL_GameControllerAxis b);
    input::GamecontrollerAxis ToAxis(SDL_GameControllerAxis b);
}

