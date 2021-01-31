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

MouseDef::MouseDef(const config::MouseDef& data)
    : detection_button(data.detection_button)
{
}


std::optional<std::string> MouseDef::ValidateKey(int key)
{
    switch(FromIndex<MouseButton>(key))
    {
        case MouseButton::LEFT:
        case MouseButton::MIDDLE:
        case MouseButton::RIGHT:
        case MouseButton::X1:
        case MouseButton::X2:
            return std::nullopt;
        default:
            return fmt::format("Invalid mouse button: {}", key);
    }
}


std::optional<std::string> MouseDef::ValidateAxis(AxisType type, int target, int axis)
{
    if(target != 0) { return fmt::format("Invalid target: {}", target); }
    switch (type)
    {
        case AxisType::GeneralAxis:
        case AxisType::Wheel:
            switch(FromIndex<Axis>(axis))
            {
                case Axis::X:
                case Axis::Y:
                    break;
                default:
                    return fmt::format("Invalid axis: {}", axis);
            }
            return std::nullopt;
        default:
            return "Invalid type for mouse";
    }
}


bool MouseDef::IsConsideredJoystick()
{
    return false;
}


bool MouseDef::CanDetect(InputDirector* director, UnitDiscovery discovery, UnitSetup*, Platform*)
{
    if(discovery == UnitDiscovery::FindHighest)
    {
        return true;
    }

    return director->WasJustPressed(detection_button);
}


std::unique_ptr<ActiveUnit> MouseDef::Create(InputDirector* director, const UnitSetup&)
{
    assert(director);

    return std::make_unique<MouseActiveUnit>(director);
}


}  // namespace input
