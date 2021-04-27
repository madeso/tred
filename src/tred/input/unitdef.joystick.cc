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


joystick_definition::joystick_definition(int id, const config::joystick_definition& data)
    : index(id)
    , start_button(data.start_button)
    , unit(data.unit)
{
}


std::optional<std::string> joystick_definition::validate_key(int key)
{
    if(key < 0)
    {
        return "Negative keys are not valid";
    }

    return std::nullopt;
}


std::optional<std::string> joystick_definition::validate_axis(axis_type type, int target, int axis)
{
    switch(type)
    {
        case axis_type::general_axis:
            if(target != 0) { return fmt::format("Invalid general target: {}", target);}
            if(axis < 0) { return fmt::format("Invalid general axis: {}", axis);}
            return std::nullopt;
        case axis_type::hat:
        case axis_type::ball:
            if(target < 0) { return fmt::format("Invalid hat/ball target: {}", target);}
            switch(from_index<xy_axis>(axis))
            {
                case xy_axis::x:
                case xy_axis::y:
                    break;
                default:
                    return fmt::format("Invalid hat/ball axis: {}", axis);
            }
            return std::nullopt;
        default:
            return "Invalid axis type";
    }
}


bool joystick_definition::is_considered_joystick()
{
    return true;
}


bool joystick_definition::can_detect(input_director* director, unit_discovery, unit_setup* setup, platform* platform)
{
    const auto potential_joysticks = platform->get_active_and_free_joysticks();
    for(auto joy: potential_joysticks)
    {
        const auto selected = setup->has_joystick(joy) == false && director->was_just_pressed(joy, start_button) && platform->match_unit(joy, unit);
        if(selected)
        {
            platform->start_using(joy, joystick_type::joystick);
            setup->add_joystick(index, joy);
            return true;
        }
    }

    return false;
}


std::unique_ptr<active_unit> joystick_definition::create(input_director* director, const unit_setup& setup)
{
    assert(director);

    return std::make_unique<joystick_active_unit>(setup.get_joystick(index), director);
}


}
