#include "tred/input/unitdef.mouse.h"

#include <cassert>
#include <string>
#include "fmt/format.h"

#include "tred/input/activeunit.mouse.h"
#include "tred/input/actionmap.h"
#include "tred/input/config.h"
#include "tred/input/director.h"
#include "tred/input/index.h"


namespace input
{

mouse_definition::mouse_definition(const config::mouse_definition& data)
    : detection_button(data.detection_button)
{
}


std::optional<std::string> mouse_definition::validate_key(int key)
{
    switch(from_index<mouse_button>(key))
    {
        case mouse_button::left:
        case mouse_button::middle:
        case mouse_button::right:
        case mouse_button::x1:
        case mouse_button::x2:
            return std::nullopt;
        default:
            return fmt::format("Invalid mouse button: {}", key);
    }
}


std::optional<std::string> mouse_definition::validate_axis(axis_type type, int target, int axis)
{
    if(target != 0) { return fmt::format("Invalid target: {}", target); }
    switch (type)
    {
        case axis_type::general_axis:
        case axis_type::wheel:
            switch(from_index<xy_axis>(axis))
            {
                case xy_axis::x:
                case xy_axis::y:
                    break;
                default:
                    return fmt::format("Invalid axis: {}", axis);
            }
            return std::nullopt;
        default:
            return "Invalid type for mouse";
    }
}


bool mouse_definition::is_considered_joystick()
{
    return false;
}


bool mouse_definition::can_detect(input_director* director, unit_discovery discovery, unit_setup*, platform*)
{
    if(discovery == unit_discovery::find_highest)
    {
        return true;
    }

    return director->was_just_pressed(detection_button);
}


std::unique_ptr<active_unit> mouse_definition::create(input_director* director, const unit_setup&)
{
    assert(director);

    return std::make_unique<mouse_active_unit>(director);
}


}
