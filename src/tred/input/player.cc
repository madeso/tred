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
    if(connected_units)
    {
        connected_units->update_table(table, dt);
    }
}


bool player::is_connected() const
{
    return connected_units != nullptr;
}


bool player::is_any_connection_considered_joystick() const
{
    if(connected_units)
    {
        return connected_units->is_any_connection_considered_joystick();
    }
    else
    {
        return false;
    }
}


void player::update_connection_status()
{
    if(connected_units)
    {
        if(connected_units->is_delete_scheduled())
        {
            connected_units.reset();
        }
    }
}


}  // namespace input
