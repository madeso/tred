#include "tred/input/sdl-system.h"

#include <string_view>
#include <string>
#include <array>

#include "SDL.h"

#include "tred/log.h"
#include "tred/input/system.h"
#include "tred/input/sdl-convert.h"


namespace
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


std::string_view ToString(Power power)
{
    switch(power)
    {
        case Power::UNKNOWN: return "Unknown";
        case Power::EMPTY: return "Empty";
        case Power::LOW: return "Low";
        case Power::MEDIUM: return "Medium";
        case Power::FULL: return "Full";
        case Power::WIRED: return "Wired";
        case Power::MAX: return "Max";
        default: return "<unknown>";
    }
}


struct Joystick
{
    explicit Joystick(int device_index)
        : joystick(SDL_JoystickOpen(device_index))
        , own_joystick(true)
    {
        if(SDL_JoystickGetAttached(joystick) == SDL_FALSE)
        {
            joystick = nullptr;
        }
    }

    explicit Joystick(SDL_Joystick* another_joystick)
        : joystick(another_joystick)
        , own_joystick(false)
    {
    }

    ~Joystick()
    {
        if(joystick && own_joystick)
        {
            SDL_JoystickClose(joystick);
        }
    }

    std::string GetName()
    {
        if(joystick)
        {
            const char* name = SDL_JoystickName(joystick);

            if(name == nullptr) { return "<no_name>"; }
            else return name;
        }
        else return "<not_attached>";
    }

    SDL_JoystickID GetDeviceIndex()
    {
        if(joystick)
        {
            const auto device_index = SDL_JoystickInstanceID(joystick);
            return device_index;
        }

        return -1;
    }

    Power GetPowerLevel()
    {
        const auto power = SDL_JoystickCurrentPowerLevel(joystick);
        switch(power)
        {
            case SDL_JOYSTICK_POWER_UNKNOWN: return Power::UNKNOWN;
            case SDL_JOYSTICK_POWER_EMPTY: return Power::EMPTY;
            case SDL_JOYSTICK_POWER_LOW: return Power::LOW;
            case SDL_JOYSTICK_POWER_MEDIUM: return Power::MEDIUM;
            case SDL_JOYSTICK_POWER_FULL: return Power::FULL;
            case SDL_JOYSTICK_POWER_WIRED: return Power::WIRED;
            case SDL_JOYSTICK_POWER_MAX: return Power::MAX;
            default: return Power::UNKNOWN;
        }
    }

    int GetNumberOfAxes()
    {
        if(joystick) return SDL_JoystickNumAxes(joystick);
        else return -1;
    }
    
    int GetNumberOfBalls()
    {
        if(joystick) return SDL_JoystickNumBalls(joystick);
        else return -1;
    }

    int GetNumberOfButtons()
    {
        if(joystick) return SDL_JoystickNumButtons(joystick);
        else return -1;
    }

    int GetNumberOfHats()
    {
        if(joystick) return SDL_JoystickNumHats(joystick);
        else return -1;
    }

    std::string GetGuid()
    {
        if(joystick)
        {
            const auto guid = SDL_JoystickGetGUID(joystick);
            constexpr std::size_t buffer_size = 256;
            auto buffer = std::array<char, buffer_size>{0,};

            SDL_JoystickGetGUIDString(guid, &buffer[0], static_cast<int>(buffer_size));

            return &buffer[0];
        }
        else return "<no_joystick>";
    }

    SDL_Joystick* joystick;
    bool own_joystick;
};


struct Haptic
{
    Haptic(Joystick* joystick)
        : haptic(SDL_HapticOpenFromJoystick(joystick->joystick))
    {
    }

    ~Haptic()
    {
        if(haptic != nullptr)
        {
            SDL_HapticClose(haptic);
        }
    }

    bool HasHaptic()
    {
        return haptic != nullptr;
    }

    bool HasSine()
    {
        if(haptic == nullptr) { return false; }
        const auto query = SDL_HapticQuery(haptic);
        return 0 != (query & SDL_HAPTIC_SINE);
    }

    std::string_view GetStatus()
    {
        if(HasHaptic() == false) { return "no haptic"; }
        if(HasSine() == false) { return "has haptics"; }
        else return "has (sine) haptics";
    }

     SDL_Haptic* haptic;
};


struct GameController
{
    GameController(int device_index)
        : controller(SDL_GameControllerOpen(device_index))
    {
        if(controller == nullptr)
        {
            LOG_ERROR("Could not open gamecontroller {}: {}", device_index, SDL_GetError());
        }
    }

    ~GameController()
    {
        if(controller)
        {
            SDL_GameControllerClose(controller);
        }
    }

    std::string GetMapping()
    {
        if(controller == nullptr) { return ""; }
        char* mapping = SDL_GameControllerMapping(controller);
        if(mapping == nullptr) { return "<unnamed_mapping>"; }
        const std::string ret = mapping;
        SDL_free(mapping);
        return ret;
    }

    Joystick GetJoystick()
    {
        SDL_Joystick* joystick = controller ? SDL_GameControllerGetJoystick(controller) : nullptr;
        return Joystick{joystick};
    }

    SDL_GameController* controller;
};


void LogInfoAboutJoystick(Joystick* joy)
{
    LOG_INFO("  Name: {}", joy->GetName());
    LOG_INFO("  Device: {}", joy->GetDeviceIndex());
    LOG_INFO("  Axes: {}", joy->GetNumberOfAxes());
    LOG_INFO("  Balls: {}", joy->GetNumberOfBalls());
    LOG_INFO("  Buttons: {}", joy->GetNumberOfButtons());
    LOG_INFO("  Hats: {}", joy->GetNumberOfHats());
    LOG_INFO("  Power: {}", ToString(joy->GetPowerLevel()));
    {
        auto haptics = Haptic{joy};
        LOG_INFO("  Haptic: {}", haptics.GetStatus());
    }
    LOG_INFO("  Guid: {}", joy->GetGuid());
}


void LogInfoAboutController(GameController* controller)
{
    // Works but it's kinda long
    // LOG_INFO("  Mapping: {}", controller->GetMapping());

    auto joy = controller->GetJoystick();
    LogInfoAboutJoystick(&joy);
}


void LogInfoAboutJoystick(int device_index)
{
    const bool is_game_controller = SDL_IsGameController(device_index);
    
    if(is_game_controller)
    {
        LOG_INFO("Joystick {} (gamecontroller)", device_index + 1);
        auto controller = GameController{device_index};
        LogInfoAboutController(&controller);
    }
    else
    {
        LOG_INFO("Joystick {}", device_index + 1);
        auto joystick = Joystick{device_index};
        LogInfoAboutJoystick(&joystick);
    }
}


}


namespace input
{


struct SdlSystemImpl
{
    SdlSystemImpl(const config::InputSystem& config)
        : system(config)
    {
        const auto number_of_joysticks = SDL_NumJoysticks();

        LOG_INFO("Joysticks found: {}", number_of_joysticks);
        for (int i = 0; i < number_of_joysticks; ++i)
        {
            LogInfoAboutJoystick(i);
        }
    }

    InputSystem system;
    std::map<SDL_JoystickID, JoystickId> sdljoystick_to_id;

    void OnEvent(const SDL_Event& event)
    {
        switch(event.type)
        {
            case SDL_KEYDOWN:
            case SDL_KEYUP:
                system.OnKeyboardKey(ToKey(event.key.keysym), event.type == SDL_KEYDOWN);
                break;

            case SDL_JOYAXISMOTION:
                {
                    auto found = sdljoystick_to_id.find(event.jaxis.which);
                    if(found == sdljoystick_to_id.end()) { return; }
                    system.OnJoystickAxis(found->second, event.jaxis.axis, event.jaxis.value/-32768.0f);
                }
                break;

            case SDL_JOYBALLMOTION:
                {
                    auto found = sdljoystick_to_id.find(event.jball.which);
                    if(found == sdljoystick_to_id.end()) { return; }
                    system.OnJoystickBall(found->second, Axis::X, event.jball.ball, event.jball.xrel);
                    system.OnJoystickBall(found->second, Axis::Y, event.jball.ball, event.jball.yrel);
                }
                break;

            case SDL_JOYHATMOTION:
                {
                    auto found = sdljoystick_to_id.find(event.jhat.which);
                    if(found == sdljoystick_to_id.end()) { return; }
                    const auto hat = GetHatValues(event.jhat.value);
                    system.OnJoystickHat(found->second, Axis::X, event.jhat.hat, static_cast<float>(hat.x));
                    system.OnJoystickHat(found->second, Axis::Y, event.jhat.hat, static_cast<float>(hat.y));
                }
                break;

            case SDL_JOYBUTTONDOWN:
            case SDL_JOYBUTTONUP:
                {
                    auto found = sdljoystick_to_id.find(event.jbutton.which);
                    if(found == sdljoystick_to_id.end()) { return; }
                    system.OnJoystickButton(found->second, event.jbutton.button, event.type == SDL_JOYBUTTONDOWN);
                }
                break;

            case SDL_JOYDEVICEADDED:
            case SDL_JOYDEVICEREMOVED:
                // HandleEvent(event.jdevice, event.type == SDL_JOYDEVICEADDED);
                // todo(Gustav): handle this
                LOG_INFO("Device {} {}", event.jdevice.which, event.type == SDL_JOYDEVICEADDED ? "added" : "removed");
                break;

            case SDL_MOUSEMOTION:
                system.OnMouseAxis(Axis::X, static_cast<float>(event.motion.xrel));
                system.OnMouseAxis(Axis::Y, static_cast<float>(event.motion.yrel));
                break;

            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
                // old-style cast error: ignore for now
                // if(event.button.which != SDL_TOUCH_MOUSEID)
                {
                    system.OnMouseButton(ToMouseButton(event.button.button), event.type == SDL_MOUSEBUTTONDOWN);
                }
                break;

            case SDL_MOUSEWHEEL:
                {
                    const auto direction = event.wheel.direction == SDL_MOUSEWHEEL_FLIPPED ? -1.0f : 1.0f;
                    if(event.wheel.x != 0)
                    {
                        system.OnMouseWheel(Axis::X, static_cast<float>(event.wheel.x) * direction);
                    }
                    if(event.wheel.y != 0)
                    {
                        system.OnMouseWheel(Axis::Y, static_cast<float>(event.wheel.y) * direction);
                    }
                }
                break;

            default:
                return;
        }
    }
};


SdlSystem::SdlSystem(const config::InputSystem& config)
    : impl(std::make_unique<SdlSystemImpl>(config))
{
}


SdlSystem::~SdlSystem()
{
}


void SdlSystem::OnEvent(const SDL_Event& event)
{
    impl->OnEvent(event);
}


}
