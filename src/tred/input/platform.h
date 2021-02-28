#pragma once

#include <vector>
#include <string>

#include "tred/types.h"


namespace input
{

enum class JoystickId : u64 {};

enum class JoystickType
{
    Joystick, GameController
};

struct Platform
{
    virtual ~Platform() = default;

    virtual std::vector<JoystickId> ActiveAndFreeJoysticks() = 0;

    virtual void StartUsing(JoystickId joy, JoystickType type) = 0;

    virtual bool MatchUnit(JoystickId joy, const std::string& unit) = 0;

    virtual void OnChangedConnection(const std::string& config) = 0;
};

}
