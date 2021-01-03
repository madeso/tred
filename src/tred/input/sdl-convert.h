#pragma once

#include "SDL.h"
#include "glm/glm.hpp"

#include "tred/input/key.h"


namespace input
{
    glm::ivec2 GetHatValues(SDL_Joystick* joystick, int hat_index);
    
    Key ToKey(SDL_Keysym key);

    MouseButton ToKey(SDL_MouseButtonEvent mb);
}

