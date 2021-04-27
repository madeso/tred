#include "tred/input/connectedunits.h"

#include <cassert>
#include <string>
#include <algorithm>

#include "tred/cint.h"

#include "tred/input/bind.h"
#include "tred/input/activeunit.h"
#include "tred/input/bind.h"


namespace input
{


connected_units::connected_units()
{
}

connected_units::~connected_units()
{
}


void connected_units::add(std::unique_ptr<active_unit>&& unit)
{
    assert(unit);
    units.push_back(std::move(unit));
}


void connected_units::add(std::unique_ptr<active_bind>&& bind)
{
    assert(bind);
    binds.push_back(std::move(bind));
}


void connected_units::update_table(table* table, float dt)
{
    assert(table);

    assert(units.size() > 0);

    for(auto& bind: binds)
    {
        bind->set_value_in_table(table, dt);
    }
}


bool connected_units::is_empty() const
{
    return units.empty();
}


bool connected_units::is_any_connection_considered_joystick()
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


bool connected_units::is_delete_scheduled()
{
    return std::any_of(units.begin(), units.end(), [](std::unique_ptr<active_unit>& u) -> bool
    {
        return u->is_delete_scheduled();
    });
}


active_unit* connected_units::get_unit(int index)
{
    return units[Cint_to_sizet(index)].get();
}


}  // namespace input
