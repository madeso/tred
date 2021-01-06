#include "tred/input/connectedunits.h"

#include <cassert>
#include <string>

#include "tred/input/bind.h"
#include "tred/input/activeunit.h"


namespace input
{


ConnectedUnits::ConnectedUnits(const ConverterDef& c)
    : converter(c)
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


void ConnectedUnits::UpdateTable(Table* table)
{
    assert(table);

    assert(units.size() > 0);

    converter.SetTable(table);
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


}  // namespace input
