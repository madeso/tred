#pragma once

#include <memory>
#include <functional>

#include "SDL.h"

#include "tred/input/platform.h"

#include "tred/types.h"
#include "glm/glm.hpp"


// todo(Gustav): move to window.cc?

namespace input
{

struct InputSystem;

struct SdlPlatformImpl;

struct SdlPlatform : public Platform
{
    SdlPlatform();
    ~SdlPlatform();

    void OnEvent(InputSystem* system, const SDL_Event& event, std::function<glm::ivec2 (u32)> window_size);

    std::vector<JoystickId> ActiveAndFreeJoysticks() override;
    void StartUsing(JoystickId joy) override;
    bool MatchUnit(JoystickId joy, const std::string& unit) override;

    std::unique_ptr<SdlPlatformImpl> impl;
};

}
