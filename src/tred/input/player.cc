#include "tred/input/player.h"

#include <cassert>

#include "tred/input/connectedunits.h"


namespace input
{


Player::Player() = default;
Player::~Player() = default;


void Player::update_table(Table* table, float dt) const
{
    assert(table);
    if(units)
    {
        units->update_table(table, dt);
    }
}


bool Player::is_connected() const
{
    return units != nullptr;
}


bool Player::is_any_connection_considered_joystick() const
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


void Player::update_connection_status()
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
