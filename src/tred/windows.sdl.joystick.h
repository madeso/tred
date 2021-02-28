#pragma once

#include <string>
#include <string_view>
#include <array>

#include "SDL.h"

#include "tred/array.h"


namespace sdl
{


constexpr auto valid_buttons = ValuesInEnum<SDL_GameControllerButton, SDL_CONTROLLER_BUTTON_MAX>();
constexpr auto valid_axes = ValuesInEnum<SDL_GameControllerAxis, SDL_CONTROLLER_AXIS_MAX>();

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
    explicit Joystick(int joystick_index);
    explicit Joystick(SDL_Joystick* another_joystick);
    ~Joystick();

    void ClearJoystick();

    explicit Joystick(Joystick&& j);
    void operator=(Joystick&& j);

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
    GameController(int joystick_index);
    ~GameController();

    bool IsValid() const;
    std::string GetMapping();

    Joystick GetJoystick();

    SDL_GameController* controller;
};


struct GamecontrollerState
{
    std::array<bool, SDL_CONTROLLER_BUTTON_MAX> buttons;
    std::array<Sint16, SDL_CONTROLLER_AXIS_MAX> axes;

    static GamecontrollerState GetState(GameController* controller);
};


void LogInfoAboutJoystick(Joystick* joy);


void LogInfoAboutController(GameController* controller);


void LogInfoAboutJoystick(int joystick_index);


}

