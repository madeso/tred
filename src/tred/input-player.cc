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
    units->UpdateTable(table);
}


void Player::Update(float dt)
{
    units->Update(dt);
}


}  // namespace input
