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
    assert(units);
    units->UpdateTable(table);
}


void Player::Update(float dt)
{
    assert(units);
    units->Update(dt);
}


}  // namespace input
