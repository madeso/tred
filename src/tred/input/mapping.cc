#include "tred/input/mapping.h"

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
    if(definitions.empty())
    {
        return false;
    }

    auto non_joystick_detected = false;

    for (auto& def: definitions)
    {
        const auto already_detected = def->IsConsideredJoystick() == false && non_joystick_detected == true;
        const auto can_detect = already_detected || def->CanDetect(director, discovery, setup, platform);

        if(can_detect == false)
        {
            return false;
        }

        non_joystick_detected = true;
    }

    // only one non-keyboard need to be detected
    // all joysticks need to be detected
    // bug: keyboard+joystick and joystick+keyboard works differently
    // bug: keyboard+mouse and mouse+keyboard works differently

    return true;
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
