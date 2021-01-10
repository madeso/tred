#pragma once

#include <memory>

#include "SDL.h"

#include "tred/input/platform.h"


namespace input
{

struct InputSystem;

struct SdlPlatformImpl;

struct SdlPlatform : public Platform
{
    SdlPlatform();
    ~SdlPlatform();

    void OnEvent(InputSystem* system, const SDL_Event& event);
    
    std::vector<JoystickId> ActiveAndFreeJoysticks() override;
    void StartUsing(JoystickId joy) override;
    bool MatchUnit(JoystickId joy, const std::string& unit) override;

    std::unique_ptr<SdlPlatformImpl> impl;
};

}
