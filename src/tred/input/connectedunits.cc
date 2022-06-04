#include "tred/input/connectedunits.h"

#include "tred/assert.h"



#include "tred/cint.h"

#include "tred/input/bind.h"
#include "tred/input/activeunit.h"
#include "tred/input/bind.h"


namespace input
{


ConnectedUnits::ConnectedUnits()
{
}

ConnectedUnits::~ConnectedUnits()
{
}


void ConnectedUnits::add(std::unique_ptr<ActiveUnit>&& unit)
{
    ASSERT(unit);
    units.push_back(std::move(unit));
}


void ConnectedUnits::add(std::unique_ptr<ActiveBind>&& bind)
{
    ASSERT(bind);
    binds.push_back(std::move(bind));
}


void ConnectedUnits::update_table(Table* table, float dt)
{
    ASSERT(table);

    ASSERT(units.size() > 0);

    for(auto& bind: binds)
    {
        bind->set_value_in_table(table, dt);
    }
}


bool ConnectedUnits::is_empty() const
{
    return units.empty();
}


bool ConnectedUnits::is_any_connection_considered_joystick()
{
    for(auto& u: units)
    {
        if( u->is_considered_joystick() )
        {
            return true;
        }
    }

    return false;
}


bool ConnectedUnits::is_delete_scheduled()
{
    return std::any_of(units.begin(), units.end(), [](std::unique_ptr<ActiveUnit>& u) -> bool
    {
        return u->is_delete_scheduled();
    });
}


ActiveUnit* ConnectedUnits::get_unit(int index)
{
    return units[Cint_to_sizet(index)].get();
}


}  // namespace input
