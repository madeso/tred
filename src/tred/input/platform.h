#pragma once




#include "tred/types.h"


namespace input
{

enum class JoystickId : u64 {};

enum class JoystickType
{
    joystick, game_controller
};

struct Platform
{
    virtual ~Platform() = default;

    virtual std::vector<JoystickId> get_active_and_free_joysticks() = 0;

    virtual void start_using(JoystickId joy, JoystickType type) = 0;

    virtual bool match_unit(JoystickId joy, const std::string& unit) = 0;

    virtual void on_changed_connection(const std::string& config) = 0;
};

}
