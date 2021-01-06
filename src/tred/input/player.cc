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


void Player::UpdateTable(Table* table)
{
    assert(table);
    if(connected_units)
    {
        connected_units->UpdateTable(table);
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


}  // namespace input
