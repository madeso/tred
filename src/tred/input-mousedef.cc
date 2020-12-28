#include "tred/input-mousedef.h"

#include <cassert>
#include <string>
#include "fmt/format.h"

#include "tred/input-mouseactiveunit.h"
#include "tred/input-actionmap.h"
#include "tred/input-config.h"


namespace input
{

MouseDef::MouseDef(const config::MouseDef& data, Converter* converter)
{
    for (const auto& d: data.axis)
    {
        axes.push_back(BindDef<Axis>(d.bindname, d.axis, d, converter));
    }

    for (const auto& d: data.button)
    {
        keys.push_back(BindDef<MouseButton>(d.bindname, d.key, d, converter));
    }
}


std::shared_ptr<ActiveUnit> MouseDef::Create(InputDirector* director)
{
    assert(director);

    std::shared_ptr<ActiveUnit> unit(new MouseActiveUnit(director, axes, keys));
    
    return unit;
}


}  // namespace input
