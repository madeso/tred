#include "tred/input/unitsetup.h"

#include <cassert>


namespace input
{


bool unit_setup::has_joystick(joystick_id joy) const
{
    return joysticks.find(joy) != joysticks.end();
}


joystick_id unit_setup::get_joystick(int index) const
{
    for(const auto& p: joysticks)
    {
        if(p.second == index)
        {
            return p.first;
        }
    }

    assert(false && "invalid index");
    return static_cast<joystick_id>(0);
}


void unit_setup::add_joystick(int index, joystick_id joy)
{
    joysticks.insert({joy, index});
}


}
