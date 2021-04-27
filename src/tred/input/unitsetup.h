#pragma once

#include <map>

#include "tred/input/platform.h"


namespace input
{

struct unit_setup
{
    [[nodiscard]] bool has_joystick(joystick_id joy) const;
    [[nodiscard]] joystick_id get_joystick(int index) const;

    void add_joystick(int index, joystick_id joy);

    std::map<joystick_id, int> joysticks;
};

}

