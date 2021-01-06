#include "tred/input/mousedef.h"

#include <cassert>
#include <string>
#include "fmt/format.h"

#include "tred/input/mouseactiveunit.h"
#include "tred/input/actionmap.h"
#include "tred/input/config.h"
#include "tred/input/director.h"


namespace input
{

MouseDef::MouseDef(const config::MouseDef& data, ConverterDef* converter)
    : detection_button(data.detection_button)
{
    for (const auto& d: data.axis)
    {
        axes.push_back(BindDef<Axis>(d.bindname, d.axis, d, converter));
    }

    for (const auto& d: data.wheel)
    {
        wheels.push_back(BindDef<Axis>(d.bindname, d.axis, d, converter));
    }

    for (const auto& d: data.button)
    {
        keys.push_back(BindDef<MouseButton>(d.bindname, d.key, d, converter));
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


std::unique_ptr<ActiveUnit> MouseDef::Create(InputDirector* director, const UnitSetup&, Converter* converter)
{
    assert(director);

    return std::make_unique<MouseActiveUnit>(director, converter, axes, wheels, keys);
}


}  // namespace input
