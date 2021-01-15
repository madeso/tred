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


Mapping::Mapping(const InputActionMap&, const config::Mapping&)
{
}


Mapping::~Mapping()
{
}


void Mapping::Add(std::unique_ptr<UnitDef>&& unit)
{
    assert(unit);
    units.push_back(std::move(unit));
}


void Mapping::Add(std::unique_ptr<BindDef>&& bind)
{
    assert(bind);
    binds.push_back(std::move(bind));
}


bool Mapping::IsAnyConsideredJoystick()
{
    for (auto& def: units)
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
        units,
        [&](const std::unique_ptr<UnitDef>& def) -> bool { return def->IsConsideredJoystick(); },
        [&](const std::unique_ptr<UnitDef>& def) -> bool { return def->CanDetect(director, discovery, setup, platform); }
    );
}


std::unique_ptr<ConnectedUnits> Mapping::Connect(InputDirector* director, const UnitSetup& setup)
{
    assert(director);
    auto connected = std::make_unique<ConnectedUnits>();

    for (auto& def: units)
    {
        auto unit = def->Create(director, setup);
        assert(unit);
        connected->Add(std::move(unit));
    }

    for(auto& bind: binds)
    {
        auto created_bind = bind->Create(connected.get());
        assert(created_bind);
        connected->Add(std::move(created_bind));
    }

    return connected;
}


}  // namespace input
