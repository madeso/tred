#include "tred/input/keyconfig.h"

#include <cassert>

#include "tred/input/unitdef.h"
#include "tred/input/director.h"
#include "tred/input/actionmap.h"
#include "tred/input/action.h"
#include "tred/input/activeunit.h"
#include "tred/input/connectedunits.h"
#include "tred/input/config.h"

namespace input
{

Mapping::Mapping(const InputActionMap& map, const config::Mapping& root)
{
    for(const auto& action_pair: map.actions)
    {
        const auto& action = action_pair.second;
        converter.AddOutput(action->name, action->scriptvarname, action->range);
    }

    for(const auto& conv: root.two_button_converter)
    {
        converter.AddTwoButton(conv);
    }
}


Mapping::~Mapping()
{
}


void Mapping::Add(std::unique_ptr<UnitDef>&& def)
{
    assert(def);
    definitions.push_back(std::move(def));
}


std::unique_ptr<ConnectedUnits> Mapping::Connect(InputDirector* director)
{
    assert(director);
    auto units = std::make_unique<ConnectedUnits>(converter);

    for (auto& def: definitions)
    {
        auto unit = def->Create(director, &units->converter);
        assert(unit);
        units->Add(std::move(unit));
    }

    return units;
}


}  // namespace input
