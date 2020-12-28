#include "tred/input-mousedef.h"

#include <cassert>
#include <string>
#include "fmt/format.h"

#include "tred/input-mouseactiveunit.h"
#include "tred/input-actionmap.h"
#include "tred/input-bindmap.h"
#include "tred/input-commondef.h"
#include "tred/input-config.h"


namespace input
{

MouseDef::MouseDef(const config::MouseDef& data, const InputActionMap&)
{
    for (const auto& d: data.axis)
    {
        axis.push_back(BindDef<Axis>(d.bindname, d.axis, d));
    }

    for (const auto& d: data.button)
    {
        keys.push_back(BindDef<MouseButton>(d.bindname, d.key, d));
    }
}


std::shared_ptr<ActiveUnit> MouseDef::Create(InputDirector* director, BindMap* map)
{
    assert(director);
    assert(map);

    std::vector<std::shared_ptr<TBind<Axis>>> axisbinds = CreateBinds<TBind<Axis>, Axis>(axis, map);
    std::vector<std::shared_ptr<TBind<MouseButton>>> keybinds = CreateBinds<TBind<MouseButton>, MouseButton>(keys, map);

    std::shared_ptr<ActiveUnit> unit(new MouseActiveUnit(axisbinds, keybinds, director));
    
    return unit;
}


}  // namespace input
