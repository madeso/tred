#include "tred/input-keyboarddef.h"

#include <cassert>
#include <stdexcept>
#include <string>
#include "fmt/format.h"

#include "tred/input-config.h"
#include "tred/input-keyboardactiveunit.h"
#include "tred/input-bindmap.h"
#include "tred/input-commondef.h"


namespace input
{

KeyboardDef::KeyboardDef(const config::KeyboardDef& data, const InputActionMap&)
{
    for (const auto& d: data.binds)
    {
        auto common = d.common;

        const auto key = d.key;
        keys_.push_back(BindDef<Key>(common.bindname, key, d));
    }
}


std::shared_ptr<ActiveUnit> KeyboardDef::Create(InputDirector* director, BindMap* map)
{
    assert(director);
    assert(map);

    std::vector<std::shared_ptr<TRangeBind<Key>>> keybinds = CreateBinds<TRangeBind<Key>, Key>(keys_, map);

    std::shared_ptr<ActiveUnit> unit(new KeyboardActiveUnit(keybinds, director));
    return unit;
}


}  // namespace input
