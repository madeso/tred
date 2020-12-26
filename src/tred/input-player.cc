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
    units_->UpdateTable(table);
}


void Player::Update(float dt)
{
    units_->Update(dt);
}


void Player::set_units(std::shared_ptr<ConnectedUnits> units)
{
    assert(units);
    assert(units->IsEmpty() == false);
    units_ = units;
}

}  // namespace input
