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

MouseUnitDef::MouseUnitDef(const config::MouseDefinition& data)
    : detection_button(data.detection_button)
{
}


std::optional<std::string> MouseUnitDef::validate_key(int key)
{
    switch(from_index<MouseButton>(key))
    {
        case MouseButton::left:
        case MouseButton::middle:
        case MouseButton::right:
        case MouseButton::x1:
        case MouseButton::x2:
            return std::nullopt;
        default:
            return fmt::format("Invalid mouse button: {}", key);
    }
}


std::optional<std::string> MouseUnitDef::validate_axis(AxisType type, int target, int axis)
{
    if(target != 0) { return fmt::format("Invalid target: {}", target); }
    switch (type)
    {
        case AxisType::general_axis:
        case AxisType::wheel:
            switch(from_index<Axis2>(axis))
            {
                case Axis2::x:
                case Axis2::y:
                    break;
                default:
                    return fmt::format("Invalid axis: {}", axis);
            }
            return std::nullopt;
        default:
            return "Invalid type for mouse";
    }
}


bool MouseUnitDef::is_considered_joystick()
{
    return false;
}


bool MouseUnitDef::can_detect(Director* director, unit_discovery discovery, UnitSetup*, Platform*)
{
    if(discovery == unit_discovery::find_highest)
    {
        return true;
    }

    return director->was_just_pressed(detection_button);
}


std::unique_ptr<ActiveUnit> MouseUnitDef::create(Director* director, const UnitSetup&)
{
    assert(director);

    return std::make_unique<MouseActiveUnit>(director);
}


}
