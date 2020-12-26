#pragma once

#include <memory>

#include "tred/input-connectedunits.h"


struct Table;


namespace input
{
/** Represents a player.
The idea behind decoupling the active units and the player is that the unit
could be disconnected and swapped but the player should remain.
*/
struct Player
{
    Player();

    void UpdateTable(Table* table);
    void Update(float dt);
    
    void set_units(std::shared_ptr<ConnectedUnits> units);

    std::shared_ptr<ConnectedUnits> units_;
};

}  // namespace input
