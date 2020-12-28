#include "tred/input-keyconfig.h"

#include <cassert>

#include "tred/input-unitdef.h"
#include "tred/input-director.h"
#include "tred/input-actionmap.h"
#include "tred/input-action.h"

namespace input
{

KeyConfig::KeyConfig(const InputActionMap& map)
{
    auto actions = map.GetActionList();
    for(const auto& action: actions)
    {
        converter.AddOutput(action->name, action->scriptvarname, action->range);
    }
}


void KeyConfig::Add(std::shared_ptr<UnitDef> def)
{
    assert(def);
    definitions.push_back(def);
}


std::shared_ptr<ConnectedUnits> KeyConfig::Connect(InputDirector* director)
{
    assert(director);

    std::shared_ptr<ConnectedUnits> units(new ConnectedUnits(converter));
    for (auto def: definitions)
    {
        auto unit = def->Create(director);
        assert(unit);
        units->Add(unit);
    }

    return units;
}


}  // namespace input
