#pragma once

#include <memory>

#include "SDL.h"


namespace input
{

struct InputSystem;

struct SdlSystemImpl;

struct SdlSystem
{
    SdlSystem(InputSystem* system);
    ~SdlSystem();

    void OnEvent(const SDL_Event& event);

    std::unique_ptr<SdlSystemImpl> impl;
};

}
