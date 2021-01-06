#include "tred/input/unitsetup.h"

#include <cassert>


namespace input
{


bool UnitSetup::HasJoystick(JoystickId joy) const
{
    return joysticks.find(joy) != joysticks.end();
}


JoystickId UnitSetup::GetJoystick(int index) const
{
    for(const auto& p: joysticks)
    {
        if(p.second == index)
        {
            return p.first;
        }
    }

    assert(false && "invalid index");
    return static_cast<JoystickId>(0);
}


void UnitSetup::AddJoystick(int index, JoystickId joy)
{
    joysticks.insert({joy, index});
}


}
