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


mapping::mapping(const input_action_map&, const config::mapping&)
{
}


mapping::~mapping()
{
}


void mapping::add(std::unique_ptr<unit_definition>&& unit)
{
    assert(unit);
    units.push_back(std::move(unit));
}


void mapping::add(std::unique_ptr<bind_definition>&& bind)
{
    assert(bind);
    binds.push_back(std::move(bind));
}


bool mapping::is_any_considered_joystick()
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


bool mapping::can_detect(input_director* director, unit_discovery discovery, unit_setup* setup, platform* platform)
{
    return mapping_detection
    (
        units,
        [&](const std::unique_ptr<unit_definition>& def) -> bool { return def->is_considered_joystick(); },
        [&](const std::unique_ptr<unit_definition>& def) -> bool { return def->can_detect(director, discovery, setup, platform); }
    );
}


std::unique_ptr<connected_units> mapping::connect(input_director* director, const unit_setup& setup)
{
    assert(director);
    auto connected = std::make_unique<connected_units>();

    for (auto& def: units)
    {
        auto unit = def->create(director, setup);
        assert(unit);
        connected->add(std::move(unit));
    }

    for(auto& bind: binds)
    {
        auto created_bind = bind->create(connected.get());
        assert(created_bind);
        connected->add(std::move(created_bind));
    }

    return connected;
}


}  // namespace input
