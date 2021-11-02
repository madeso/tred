#pragma once

#include <map>

#include "tred/input/platform.h"


namespace input
{

struct UnitSetup
{
    [[nodiscard]] bool has_joystick(JoystickId joy) const;
    [[nodiscard]] JoystickId get_joystick(int index) const;

    void add_joystick(int index, JoystickId joy);

    std::map<JoystickId, int> joysticks;
};

}

