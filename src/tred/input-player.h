/** @file
Classes for input handling.
 */

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
    /** Constructor.
   */
    Player();

    /** Updates all connected units.
  @param table the table to update
  */
    void UpdateTable(Table* table);

    void Update(float dt);

    /** Sets the connected units.
  @param units the units
  */
    void set_units(std::shared_ptr<ConnectedUnits> units);

    std::shared_ptr<ConnectedUnits> units_;
};

}  // namespace input
