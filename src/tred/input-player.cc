#include "tred/input-player.h"

#include <cassert>


namespace input
{


Player::Player()
{
}


void Player::UpdateTable(Table* table)
{
    assert(table);
    if(units)
    {
        units->UpdateTable(table);
    }
}


void Player::Update(float dt)
{
    if(units)
    {
        units->Update(dt);
    }
}


}  // namespace input
