#include "tred/input/unitdef.joystick.h"

#include "tred/assert.h"



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


JoystickUnitDef::JoystickUnitDef(int id, const config::JoystickDefinition& data)
    : index(id)
    , start_button(data.start_button)
    , unit(data.unit)
{
}


std::optional<std::string> JoystickUnitDef::validate_key(int key)
{
    if(key < 0)
    {
        return "Negative keys are not valid";
    }

    return std::nullopt;
}


std::optional<std::string> JoystickUnitDef::validate_axis(AxisType type, int target, int axis)
{
    switch(type)
    {
        case AxisType::general_axis:
            if(target != 0) { return fmt::format("Invalid general target: {}", target);}
            if(axis < 0) { return fmt::format("Invalid general axis: {}", axis);}
            return std::nullopt;
        case AxisType::hat:
        case AxisType::ball:
            if(target < 0) { return fmt::format("Invalid hat/ball target: {}", target);}
            switch(from_index<Axis2>(axis))
            {
                case Axis2::x:
                case Axis2::y:
                    break;
                default:
                    return fmt::format("Invalid hat/ball axis: {}", axis);
            }
            return std::nullopt;
        default:
            return "Invalid axis type";
    }
}


bool JoystickUnitDef::is_considered_joystick()
{
    return true;
}


bool JoystickUnitDef::can_detect(Director* director, unit_discovery, UnitSetup* setup, Platform* platform)
{
    const auto potential_joysticks = platform->get_active_and_free_joysticks();
    for(auto joy: potential_joysticks)
    {
        const auto selected = setup->has_joystick(joy) == false && director->was_just_pressed(joy, start_button) && platform->match_unit(joy, unit);
        if(selected)
        {
            platform->start_using(joy, JoystickType::joystick);
            setup->add_joystick(index, joy);
            return true;
        }
    }

    return false;
}


std::unique_ptr<ActiveUnit> JoystickUnitDef::create(Director* director, const UnitSetup& setup)
{
    ASSERT(director);

    return std::make_unique<JoystickActiveUnit>(setup.get_joystick(index), director);
}


}
