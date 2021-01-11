#include "tred/input/mapping.h"

#include <cassert>
#include <algorithm>

#include "tred/input/unitdef.h"
#include "tred/input/director.h"
#include "tred/input/actionmap.h"
#include "tred/input/action.h"
#include "tred/input/activeunit.h"
#include "tred/input/connectedunits.h"
#include "tred/input/config.h"
#include "tred/input/mapping.detection.h"


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


bool Mapping::IsAnyConsideredJoystick()
{
    for (auto& def: definitions)
    {
        if( def->IsConsideredJoystick() )
        {
            return true;
        }
    }

    return false;
}


bool Mapping::CanDetect(InputDirector* director, UnitDiscovery discovery, UnitSetup* setup, Platform* platform)
{
    return MappingDetection
    (
        definitions,
        [&](const std::unique_ptr<UnitDef>& def) -> bool { return def->IsConsideredJoystick(); },
        [&](const std::unique_ptr<UnitDef>& def) -> bool { return def->CanDetect(director, discovery, setup, platform); }
    );
}


std::unique_ptr<ConnectedUnits> Mapping::Connect(InputDirector* director, const UnitSetup& setup)
{
    assert(director);
    auto units = std::make_unique<ConnectedUnits>(converter);

    for (auto& def: definitions)
    {
        auto unit = def->Create(director, setup, &units->converter);
        assert(unit);
        units->Add(std::move(unit));
    }

    return units;
}


}  // namespace input
