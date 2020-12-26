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
        auto common = d.common;

        const auto axis = d.axis;
        axis_.push_back(BindDef<Axis>(common.bindname, axis, d));
    }
    for (const auto& d: data.button)
    {
        auto common = d.common;
        const auto key = d.key;

        keys_.push_back(BindDef<MouseButton>(common.bindname, key, d));
    }
}


std::shared_ptr<ActiveUnit> MouseDef::Create(InputDirector* director, BindMap* map)
{
    assert(director);
    assert(map);

    std::vector<std::shared_ptr<TAxisBind<Axis>>> axisbinds =
            CreateBinds<TAxisBind<Axis>, Axis>(axis_, map);

    std::vector<std::shared_ptr<TRangeBind<MouseButton>>> keybinds =
            CreateBinds<TRangeBind<MouseButton>, MouseButton>(keys_, map);

    std::shared_ptr<ActiveUnit> unit(
            new MouseActiveUnit(axisbinds, keybinds, director));
    return unit;
}


}  // namespace input
