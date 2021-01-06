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

    void RemoveJustPressed() override;
    std::vector<JoystickId> ActiveAndFreeJoysticks() override;
    bool MatchUnit(JoystickId joy, const std::string& unit) override;
    bool WasJustPressed(JoystickId joy, int button) override;

    std::unique_ptr<SdlPlatformImpl> impl;
};

}
