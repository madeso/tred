#pragma once

#include <vector>
#include <string>

#include "tred/types.h"


namespace input
{

enum class JoystickId : u64 {};

struct Platform
{
    virtual ~Platform() = default;

    virtual void RemoveJustPressed() = 0;
    virtual std::vector<JoystickId> ActiveAndFreeJoysticks() = 0;

    virtual bool MatchUnit(JoystickId joy, const std::string& unit) = 0;
    virtual bool WasJustPressed(JoystickId joy, int button) = 0;
};

}
