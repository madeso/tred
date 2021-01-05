#pragma once

#include <memory>

#include "SDL.h"


namespace input::config
{
    struct InputSystem;
}


namespace input
{

struct InputSystem;

struct SdlSystemImpl;

struct SdlSystem
{
    SdlSystem(const config::InputSystem& config);
    ~SdlSystem();

    void OnEvent(const SDL_Event& event);

    std::unique_ptr<SdlSystemImpl> impl;
};

}
