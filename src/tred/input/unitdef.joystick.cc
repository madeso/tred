#include "tred/input/unitdef.joystick.h"

#include <cassert>
#include <string>
#include "fmt/format.h"

#include "tred/log.h"
#include "tred/input/config.h"
#include "tred/input/bind.h"
#include "tred/input/actionmap.h"
#include "tred/input/activeunit.joystick.h"
#include "tred/input/unitsetup.h"
#include "tred/input/platform.h"
#include "tred/input/director.h"
#include "tred/input/index.h"


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


std::optional<std::string> JoystickDef::ValidateAxis(AxisType type, int target, int axis)
{
    switch(type)
    {
        case AxisType::GeneralAxis:
            if(target != 0) { return fmt::format("Invalid general target: {}", target);}
            if(axis < 0) { return fmt::format("Invalid general axis: {}", axis);}
            return std::nullopt;
        case AxisType::Hat:
        case AxisType::Ball:
            if(target < 0) { return fmt::format("Invalid hat/ball target: {}", target);}
            switch(FromIndex<Axis>(axis))
            {
                case Axis::X:
                case Axis::Y:
                    break;
                default:
                    return fmt::format("Invalid hat/ball axis: {}", axis);
            }
            return std::nullopt;
        default:
            return "Invalid axis type";
    }
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
