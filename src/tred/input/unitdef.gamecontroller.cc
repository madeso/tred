#include "unitdef.gamecontroller.h"

#include <cassert>
#include <string>
#include "fmt/format.h"

#include "tred/log.h"
#include "tred/input/config.h"
#include "tred/input/bind.h"
#include "tred/input/key.h"
#include "tred/input/axis.h"
#include "tred/input/actionmap.h"
#include "tred/input/activeunit.gamecontroller.h"
#include "tred/input/unitsetup.h"
#include "tred/input/platform.h"
#include "tred/input/director.h"
#include "tred/input/index.h"


namespace input
{


gamecontroller_definition::gamecontroller_definition(int id, const config::gamecontroller_definition&)
    : index(id)
{
}


std::optional<std::string> gamecontroller_definition::validate_key(int key)
{
    if(key <= to_index(gamecontroller_button::invalid)
    || key > to_index(gamecontroller_button::trigger_right))
    {
        return fmt::format("Invalid bind to invalid gamecontroller button: {}", key);
    }

    return std::nullopt;
}


std::optional<std::string> gamecontroller_definition::validate_axis(axis_type type, int target, int axis)
{
    if(type != axis_type::general_axis) { return fmt::format("Only general axcis for game controllers: {}", type); }
    if(target != 0) { return fmt::format("Target needs to be 0 for controller: {}", target); }

    switch(from_index<gamecontroller_axis>(axis))
    {
        case gamecontroller_axis::left_x:
        case gamecontroller_axis::left_y:
        case gamecontroller_axis::right_x:
        case gamecontroller_axis::right_y:
            return std::nullopt;
        default:
            return fmt::format("Invalid axis: {}", axis);
    }
}


bool gamecontroller_definition::is_considered_joystick()
{
    return true;
}


bool gamecontroller_definition::can_detect(input_director* director, unit_discovery, unit_setup* setup, platform* platform)
{
    const auto potential_joysticks = platform->get_active_and_free_joysticks();
    for(auto joy: potential_joysticks)
    {
        const auto selected = setup->has_joystick(joy) == false && director->was_game_controller_start_just_pressed(joy);
        if(selected)
        {
            platform->start_using(joy, joystick_type::game_controller);
            setup->add_joystick(index, joy);
            return true;
        }
    }

    return false;
}


std::unique_ptr<active_unit> gamecontroller_definition::create(input_director* director, const unit_setup& setup)
{
    assert(director);
    return std::make_unique<gamecontroller_active_unit>(setup.get_joystick(index), director);
}


}
