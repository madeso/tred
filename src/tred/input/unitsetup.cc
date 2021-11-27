#include "tred/input/unitsetup.h"

#include "tred/assert.h"


namespace input
{


bool UnitSetup::has_joystick(JoystickId joy) const
{
    return joysticks.find(joy) != joysticks.end();
}


JoystickId UnitSetup::get_joystick(int index) const
{
    for(const auto& p: joysticks)
    {
        if(p.second == index)
        {
            return p.first;
        }
    }

    ASSERT(false && "invalid index");
    return static_cast<JoystickId>(0);
}


void UnitSetup::add_joystick(int index, JoystickId joy)
{
    joysticks.insert({joy, index});
}


}
