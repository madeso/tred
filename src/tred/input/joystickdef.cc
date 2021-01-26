#include "tred/input/joystickdef.h"

#include <cassert>
#include <string>
#include "fmt/format.h"

#include "tred/log.h"
#include "tred/input/config.h"
#include "tred/input/bind.h"
#include "tred/input/actionmap.h"
#include "tred/input/joystickactiveunit.h"
#include "tred/input/unitsetup.h"
#include "tred/input/platform.h"
#include "tred/input/director.h"


namespace input
{


JoystickDef::JoystickDef(int id, const config::JoystickDef& data)
    : index(id)
    , start_button(data.start_button)
    , unit(data.unit)
{
}


std::optional<std::string> JoystickDef::ValidateKey(int key)
{
    if(key < 0)
    {
        return "Negative keys are not valid";
    }

    return std::nullopt;
}


bool JoystickDef::IsConsideredJoystick()
{
    return true;
}


bool JoystickDef::CanDetect(InputDirector* director, UnitDiscovery, UnitSetup* setup, Platform* platform)
{
    const auto potential_joysticks = platform->ActiveAndFreeJoysticks();
    for(auto joy: potential_joysticks)
    {
        const auto selected = setup->HasJoystick(joy) == false && director->WasJustPressed(joy, start_button) && platform->MatchUnit(joy, unit);
        if(selected)
        {
            platform->StartUsing(joy);
            setup->AddJoystick(index, joy);
            return true;
        }
    }

    return false;
}


std::unique_ptr<ActiveUnit> JoystickDef::Create(InputDirector* director, const UnitSetup& setup)
{
    assert(director);

    return std::make_unique<JoystickActiveUnit>(setup.GetJoystick(index), director);
}


}  // namespace input
