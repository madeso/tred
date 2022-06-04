#pragma once



#include <array>

#include "tred/dependency_sdl.h"
#include "tred/array.h"


namespace sdl
{


constexpr auto valid_buttons = values_in_enum<SDL_GameControllerButton, SDL_CONTROLLER_BUTTON_MAX>();
constexpr auto valid_axes = values_in_enum<SDL_GameControllerAxis, SDL_CONTROLLER_AXIS_MAX>();

enum class PowerLevel
{
      unknown
    , empty
    , low
    , medium
    , full
    , wired
    , max
};


std::string_view to_string(PowerLevel power);


struct Joystick
{
    explicit Joystick(int joystick_index);
    explicit Joystick(SDL_Joystick* another_joystick);
    ~Joystick();

    void ClearJoystick();

    explicit Joystick(Joystick&& j);
    void operator=(Joystick&& j);

    std::string get_name();

    SDL_JoystickID get_device_index();

    PowerLevel get_power_level();

    // expand with type? SDL_JoystickGetType

    int get_number_of_axes();

    int get_number_of_balls();

    int get_number_of_buttons();

    int get_number_of_hats();

    std::string get_guid();


    SDL_Joystick* sdl_joystick;
    bool own_joystick;
};


struct Haptic
{
    Haptic(Joystick* joystick);
    ~Haptic();

    bool has_haptic();

    bool has_sine();

    std::string_view get_status();

     SDL_Haptic* sdl_haptic;
};


struct GameController
{
    GameController(int joystick_index);
    ~GameController();

    [[nodiscard]] bool is_valid() const;
    std::string get_mapping();

    Joystick get_joystick();

    SDL_GameController* controller;
};


struct GamecontrollerState
{
    std::array<bool, SDL_CONTROLLER_BUTTON_MAX> buttons;
    std::array<Sint16, SDL_CONTROLLER_AXIS_MAX> axes;

    GamecontrollerState();
    static GamecontrollerState get_state(GameController* controller);
};


void log_info_about_joystick(Joystick* joy);


void log_info_about_controller(GameController* controller);


void log_info_about_joystick(int joystick_index);


}

