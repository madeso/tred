#include "tred/input/connectedunits.h"

#include <cassert>
#include <string>
#include <algorithm>

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


void ConnectedUnits::Add(std::unique_ptr<ActiveUnit>&& unit)
{
    assert(unit);
    units.push_back(std::move(unit));
}


void ConnectedUnits::Add(std::unique_ptr<ActiveBind>&& bind)
{
    assert(bind);
    binds.push_back(std::move(bind));
}


void ConnectedUnits::UpdateTable(Table* table)
{
    assert(table);

    assert(units.size() > 0);

    for(auto& bind: binds)
    {
        bind->SetValueInTable(table);
    }
}


bool ConnectedUnits::IsEmpty() const
{
    return units.empty();
}


bool ConnectedUnits::IsAnyConnectionConsideredJoystick()
{
    for(auto& u: units)
    {
        if( u->IsConsideredJoystick() )
        {
            return true;
        }
    }

    return false;
}


bool ConnectedUnits::IsDeleteSheduled()
{
    return std::any_of(units.begin(), units.end(), [](std::unique_ptr<ActiveUnit>& u) -> bool
    {
        return u->IsDeleteSheduled();
    });
}


ActiveUnit* ConnectedUnits::GetUnit(int index)
{
    return units[static_cast<std::size_t>(index)].get();
}


}  // namespace input
