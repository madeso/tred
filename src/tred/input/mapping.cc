#include "tred/input/mapping.h"

#include "tred/assert.h"
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


Mapping::Mapping(const ActionMap&, const config::Mapping&)
{
}


Mapping::~Mapping()
{
}


void Mapping::add(std::unique_ptr<UnitDef>&& unit)
{
    ASSERT(unit);
    units.push_back(std::move(unit));
}


void Mapping::add(std::unique_ptr<BindDefinition>&& bind)
{
    ASSERT(bind);
    binds.push_back(std::move(bind));
}


bool Mapping::is_any_considered_joystick()
{
    for (auto& def: units)
    {
        if( def->is_considered_joystick() )
        {
            return true;
        }
    }

    return false;
}


bool Mapping::can_detect(Director* director, unit_discovery discovery, UnitSetup* setup, Platform* platform)
{
    return mapping_detection
    (
        units,
        [&](const std::unique_ptr<UnitDef>& def) -> bool { return def->is_considered_joystick(); },
        [&](const std::unique_ptr<UnitDef>& def) -> bool { return def->can_detect(director, discovery, setup, platform); }
    );
}


std::unique_ptr<ConnectedUnits> Mapping::connect(Director* director, const UnitSetup& setup)
{
    ASSERT(director);
    auto connected = std::make_unique<ConnectedUnits>();

    for (auto& def: units)
    {
        auto unit = def->create(director, setup);
        ASSERT(unit);
        connected->add(std::move(unit));
    }

    for(auto& bind: binds)
    {
        auto created_bind = bind->create(connected.get());
        ASSERT(created_bind);
        connected->add(std::move(created_bind));
    }

    return connected;
}


}  // namespace input
