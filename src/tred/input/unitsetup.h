#pragma once

#include <map>

#include "tred/input/platform.h"


namespace input
{

struct UnitSetup
{
    bool HasJoystick(JoystickId joy) const;
    JoystickId GetJoystick(int index) const;

    void AddJoystick(int index, JoystickId joy);

    std::map<JoystickId, int> joysticks;
};

}

