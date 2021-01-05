#pragma once

#include "SDL.h"
#include "glm/glm.hpp"

#include "tred/input/key.h"


namespace input
{
    glm::ivec2 GetHatValues(Uint8 hat);
    
    Key ToKey(SDL_Keysym key);

    MouseButton ToMouseButton(Uint8 mb);
}

