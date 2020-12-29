#include "tred/input-connectedunits.h"

#include <cassert>
#include <string>

#include "tred/input-bind.h"
#include "tred/input-activeunit.h"


namespace input
{


ConnectedUnits::ConnectedUnits(const Converter& c)
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

    auto reciever = ValueReciever{table, converter};

    for(auto& u: units)
    {
        u->Recieve(&reciever);
    }
}


void ConnectedUnits::Update(float)
{
    // actives->Update(dt);
}


bool ConnectedUnits::IsEmpty() const
{
    return units.empty();
}


}  // namespace input
