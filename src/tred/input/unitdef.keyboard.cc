#include "tred/input/unitdef.keyboard.h"

#include <cassert>
#include <stdexcept>
#include <string>
#include "fmt/format.h"

#include "tred/input/config.h"
#include "tred/input/activeunit.keyboard.h"
#include "tred/input/director.h"
#include "tred/input/index.h"


namespace input
{

keyboard_definition::keyboard_definition(const config::keyboard_definition& data)
    : detection_key(data.detection_key)
{
}


std::optional<std::string> keyboard_definition::validate_key(int key)
{
    if(key <= to_index(keyboard_key::unbound))
    {
        return fmt::format("Invalid bind to unbound: {}", key);
    }

    if(key > to_index(keyboard_key::sleep))
    {
        return fmt::format("Invalid bind to invalid key: {}", key);
    }

    return std::nullopt;
}


std::optional<std::string> keyboard_definition::validate_axis(axis_type, int, int)
{
    return "Keyboard doesn't support axis binds";
}


bool keyboard_definition::is_considered_joystick()
{
    return false;
}


bool keyboard_definition::can_detect(input_director* director, unit_discovery discovery, unit_setup*, platform*)
{
    if(discovery == unit_discovery::find_highest)
    {
        return true;
    }

    return director->was_just_pressed(detection_key);
}


std::unique_ptr<active_unit> keyboard_definition::create(input_director* director, const unit_setup&)
{
    assert(director);

    return std::make_unique<keyboard_active_unit>(director);
}


}
