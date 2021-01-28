#pragma once

#include "SDL.h"
#include "glm/glm.hpp"

#include "tred/input/key.h"


namespace sdl
{
    glm::ivec2 GetHatValues(Uint8 hat);

    input::Key ToKey(SDL_Keysym key);

    input::MouseButton ToMouseButton(Uint8 mb);
}

