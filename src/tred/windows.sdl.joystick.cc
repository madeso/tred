#include "tred/windows.sdl.joystick.h"

#include "tred/log.h"
#include "tred/cint.h"


namespace sdl
{


std::string_view ToString(power_level power)
{
    switch(power)
    {
        case power_level::unknown: return "Unknown";
        case power_level::empty: return "Empty";
        case power_level::low: return "Low";
        case power_level::medium: return "Medium";
        case power_level::full: return "Full";
        case power_level::wired: return "Wired";
        case power_level::max: return "Max";
        default: return "<unknown>";
    }
}


joystick::joystick(int joystick_index)
    : sdl_joystick(SDL_JoystickOpen(joystick_index))
    , own_joystick(true)
{
    if(SDL_JoystickGetAttached(sdl_joystick) == SDL_FALSE)
    {
        ClearJoystick();
    }
}


joystick::joystick(SDL_Joystick* another_joystick)
    : sdl_joystick(another_joystick)
    , own_joystick(false)
{
}


joystick::~joystick()
{
    ClearJoystick();
}



void joystick::ClearJoystick()
{
    if(sdl_joystick && own_joystick)
    {
        SDL_JoystickClose(sdl_joystick);
    }

    sdl_joystick = nullptr;
    own_joystick = false;
}


joystick::joystick(joystick&& j)
    : sdl_joystick(j.sdl_joystick)
    , own_joystick(j.own_joystick)
{
    j.sdl_joystick = nullptr;
    j.own_joystick = false;
}


void joystick::operator=(joystick&& j)
{
    ClearJoystick();

    sdl_joystick = j.sdl_joystick;
    own_joystick = j.own_joystick;
    j.sdl_joystick = nullptr;
    j.own_joystick = false;
}


std::string joystick::get_name()
{
    if(sdl_joystick)
    {
        const char* name = SDL_JoystickName(sdl_joystick);

        if(name == nullptr) { return "<no_name>"; }
        else return name;
    }
    else return "<not_attached>";
}


SDL_JoystickID joystick::get_device_index()
{
    if(sdl_joystick)
    {
        const auto device_index = SDL_JoystickInstanceID(sdl_joystick);
        return device_index;
    }

    return -1;
}


power_level joystick::get_power_level()
{
    const auto power = SDL_JoystickCurrentPowerLevel(sdl_joystick);
    switch(power)
    {
        case SDL_JOYSTICK_POWER_UNKNOWN: return power_level::unknown;
        case SDL_JOYSTICK_POWER_EMPTY: return power_level::empty;
        case SDL_JOYSTICK_POWER_LOW: return power_level::low;
        case SDL_JOYSTICK_POWER_MEDIUM: return power_level::medium;
        case SDL_JOYSTICK_POWER_FULL: return power_level::full;
        case SDL_JOYSTICK_POWER_WIRED: return power_level::wired;
        case SDL_JOYSTICK_POWER_MAX: return power_level::max;
        default: return power_level::unknown;
    }
}


int joystick::get_number_of_axes()
{
    if(sdl_joystick) return SDL_JoystickNumAxes(sdl_joystick);
    else return -1;
}


int joystick::get_number_of_balls()
{
    if(sdl_joystick) return SDL_JoystickNumBalls(sdl_joystick);
    else return -1;
}


int joystick::get_number_of_buttons()
{
    if(sdl_joystick) return SDL_JoystickNumButtons(sdl_joystick);
    else return -1;
}


int joystick::get_number_of_hats()
{
    if(sdl_joystick) return SDL_JoystickNumHats(sdl_joystick);
    else return -1;
}


std::string joystick::get_guid()
{
    if(sdl_joystick)
    {
        const auto guid = SDL_JoystickGetGUID(sdl_joystick);
        constexpr std::size_t buffer_size = 256;
        auto buffer = std::array<char, buffer_size>{0,};

        SDL_JoystickGetGUIDString(guid, &buffer[0], Csizet_to_int(buffer_size));

        return &buffer[0];
    }
    else return "<no_joystick>";
}


haptic::haptic(joystick* joystick)
    : sdl_haptic(SDL_HapticOpenFromJoystick(joystick->sdl_joystick))
{
}


haptic::~haptic()
{
    if(sdl_haptic != nullptr)
    {
        SDL_HapticClose(sdl_haptic);
    }
}


bool haptic::has_haptic()
{
    return sdl_haptic != nullptr;
}


bool haptic::has_sine()
{
    if(sdl_haptic == nullptr) { return false; }
    const auto query = SDL_HapticQuery(sdl_haptic);
    return 0 != (query & SDL_HAPTIC_SINE);
}


std::string_view haptic::get_status()
{
    if(has_haptic() == false) { return "no haptic"; }
    if(has_sine() == false) { return "has haptics"; }
    else return "has (sine) haptics";
}


game_controller::game_controller(int joystick_index)
    : controller(SDL_GameControllerOpen(joystick_index))
{
    if(controller == nullptr)
    {
        LOG_ERROR("Could not open gamecontroller {}: {}", joystick_index, SDL_GetError());
    }
}


game_controller::~game_controller()
{
    if(controller)
    {
        SDL_GameControllerClose(controller);
    }
}


bool game_controller::is_valid() const
{
    return controller != nullptr;
}


std::string game_controller::get_mapping()
{
    if(controller == nullptr) { return ""; }
    char* mapping = SDL_GameControllerMapping(controller);
    if(mapping == nullptr) { return "<unnamed_mapping>"; }
    const std::string ret = mapping;
    SDL_free(mapping);
    return ret;
}


joystick game_controller::get_joystick()
{
    SDL_Joystick* joy = controller ? SDL_GameControllerGetJoystick(controller) : nullptr;
    return joystick{ joy };
}

gamecontroller_state::gamecontroller_state()
    : buttons({ false,})
    , axes({ 0, })
{
}

gamecontroller_state gamecontroller_state::get_state(game_controller* controller)
{
    gamecontroller_state r;

    for(auto button: valid_buttons)
    {
        r.buttons[static_cast<std::size_t>(button)] = SDL_GameControllerGetButton(controller->controller, button) == 1;
    }

    for(auto axis: valid_axes)
    {
        r.axes[static_cast<std::size_t>(axis)] = SDL_GameControllerGetAxis(controller->controller, axis);
    }

    return r;
}


void log_info_about_joystick(joystick* joy)
{
    LOG_INFO("  Name: {}", joy->get_name());
    LOG_INFO("  Device: {}", joy->get_device_index());
    LOG_INFO("  Axes: {}", joy->get_number_of_axes());
    LOG_INFO("  Balls: {}", joy->get_number_of_balls());
    LOG_INFO("  Buttons: {}", joy->get_number_of_buttons());
    LOG_INFO("  Hats: {}", joy->get_number_of_hats());
    LOG_INFO("  Power: {}", ToString(joy->get_power_level()));
    {
        auto haptics = haptic{joy};
        LOG_INFO("  Haptic: {}", haptics.get_status());
    }
    LOG_INFO("  Guid: {}", joy->get_guid());
}


void log_info_about_controller(game_controller* controller)
{
    // Works but it's kinda long
    // LOG_INFO("  Mapping: {}", controller->GetMapping());

    auto joy = controller->get_joystick();
    log_info_about_joystick(&joy);
}


void log_info_about_joystick(int joystick_index)
{
    const bool is_game_controller = SDL_IsGameController(joystick_index);

    if(is_game_controller)
    {
        LOG_INFO("Joystick {} (gamecontroller)", joystick_index);
        auto controller = game_controller{joystick_index};
        log_info_about_controller(&controller);
    }
    else
    {
        LOG_INFO("Joystick {}", joystick_index);
        auto j = joystick{joystick_index};
        log_info_about_joystick(&j);
    }
}


}

