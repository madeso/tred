#include "tred/input-player.h"

#include <cassert>

#include "tred/input-connectedunits.h"


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


void Player::Update(float dt)
{
    if(connected_units)
    {
        connected_units->Update(dt);
    }
}


}  // namespace input
