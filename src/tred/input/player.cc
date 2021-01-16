#include "tred/input/player.h"

#include <cassert>

#include "tred/input/connectedunits.h"


namespace input
{


Player::Player()
{
}


Player::~Player()
{
}


void Player::UpdateTable(Table* table, float dt)
{
    assert(table);
    if(connected_units)
    {
        connected_units->UpdateTable(table, dt);
    }
}


bool Player::IsConnected()
{
    return connected_units != nullptr;
}


bool Player::IsAnyConnectionConsideredJoystick()
{
    if(connected_units)
    {
        return connected_units->IsAnyConnectionConsideredJoystick();
    }
    else
    {
        return false;
    }
}


void Player::UpdateConnectionStatus()
{
    if(connected_units)
    {
        if(connected_units->IsDeleteSheduled())
        {
            connected_units.reset();
        }
    }
}


}  // namespace input
