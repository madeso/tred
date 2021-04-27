#pragma once

#include <string>
#include <string_view>
#include <array>

#include "SDL.h"

#include "tred/array.h"


namespace sdl
{


constexpr auto valid_buttons = values_in_enum<SDL_GameControllerButton, SDL_CONTROLLER_BUTTON_MAX>();
constexpr auto valid_axes = values_in_enum<SDL_GameControllerAxis, SDL_CONTROLLER_AXIS_MAX>();

enum class power_level
{
      unknown
    , empty
    , low
    , medium
    , full
    , wired
    , max
};


std::string_view ToString(power_level power);


struct joystick
{
    explicit joystick(int joystick_index);
    explicit joystick(SDL_Joystick* another_joystick);
    ~joystick();

    void ClearJoystick();

    explicit joystick(joystick&& j);
    void operator=(joystick&& j);

    std::string get_name();

    SDL_JoystickID get_device_index();

    power_level get_power_level();

    // expand with type? SDL_JoystickGetType

    int get_number_of_axes();

    int get_number_of_balls();

    int get_number_of_buttons();

    int get_number_of_hats();

    std::string get_guid();


    SDL_Joystick* sdl_joystick;
    bool own_joystick;
};


struct haptic
{
    haptic(joystick* joystick);
    ~haptic();

    bool has_haptic();

    bool has_sine();

    std::string_view get_status();

     SDL_Haptic* sdl_haptic;
};


struct game_controller
{
    game_controller(int joystick_index);
    ~game_controller();

    [[nodiscard]] bool is_valid() const;
    std::string get_mapping();

    joystick get_joystick();

    SDL_GameController* controller;
};


struct gamecontroller_state
{
    std::array<bool, SDL_CONTROLLER_BUTTON_MAX> buttons;
    std::array<Sint16, SDL_CONTROLLER_AXIS_MAX> axes;

    gamecontroller_state();
    static gamecontroller_state get_state(game_controller* controller);
};


void log_info_about_joystick(joystick* joy);


void log_info_about_controller(game_controller* controller);


void log_info_about_joystick(int joystick_index);


}

