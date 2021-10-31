#include "tred/input/player.h"

#include <cassert>

#include "tred/input/connectedunits.h"


namespace input
{


player::player() = default;
player::~player() = default;


void player::update_table(table* table, float dt) const
{
    assert(table);
    if(units)
    {
        units->update_table(table, dt);
    }
}


bool player::is_connected() const
{
    return units != nullptr;
}


bool player::is_any_connection_considered_joystick() const
{
    if(units)
    {
        return units->is_any_connection_considered_joystick();
    }
    else
    {
        return false;
    }
}


void player::update_connection_status()
{
    if(units)
    {
        if(units->is_delete_scheduled())
        {
            units.reset();
        }
    }
}


}  // namespace input
