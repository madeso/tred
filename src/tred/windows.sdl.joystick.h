#pragma once

#include <string>
#include <string_view>

#include "SDL.h"


namespace sdl
{


enum class Power
{
      UNKNOWN
    , EMPTY
    , LOW
    , MEDIUM
    , FULL
    , WIRED
    , MAX
};


std::string_view ToString(Power power);


struct Joystick
{
    explicit Joystick(int device_index);
    explicit Joystick(SDL_Joystick* another_joystick);
    ~Joystick();

    std::string GetName();

    SDL_JoystickID GetDeviceIndex();

    Power GetPowerLevel();

    int GetNumberOfAxes();

    int GetNumberOfBalls();

    int GetNumberOfButtons();

    int GetNumberOfHats();

    std::string GetGuid();

    SDL_Joystick* joystick;
    bool own_joystick;
};


struct Haptic
{
    Haptic(Joystick* joystick);
    ~Haptic();

    bool HasHaptic();

    bool HasSine();

    std::string_view GetStatus();

     SDL_Haptic* haptic;
};


struct GameController
{
    GameController(int device_index);
    ~GameController();

    std::string GetMapping();

    Joystick GetJoystick();

    SDL_GameController* controller;
};


void LogInfoAboutJoystick(Joystick* joy);


void LogInfoAboutController(GameController* controller);


void LogInfoAboutJoystick(int device_index);


}

