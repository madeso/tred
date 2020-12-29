#include "tred/input-keyconfig.h"

#include <cassert>

#include "tred/input-unitdef.h"
#include "tred/input-director.h"
#include "tred/input-actionmap.h"
#include "tred/input-action.h"
#include "tred/input-activeunit.h"
#include "tred/input-connectedunits.h"

namespace input
{

KeyConfig::KeyConfig(const InputActionMap& map)
{
    for(const auto& action_pair: map.actions)
    {
        const auto& action = action_pair.second;
        converter.AddOutput(action->name, action->scriptvarname, action->range);
    }
}


KeyConfig::~KeyConfig()
{
}


void KeyConfig::Add(std::unique_ptr<UnitDef>&& def)
{
    assert(def);
    definitions.push_back(std::move(def));
}


std::unique_ptr<ConnectedUnits> KeyConfig::Connect(InputDirector* director)
{
    assert(director);
    auto units = std::make_unique<ConnectedUnits>(converter);

    for (auto& def: definitions)
    {
        auto unit = def->Create(director);
        assert(unit);
        units->Add(std::move(unit));
    }

    return units;
}


}  // namespace input
