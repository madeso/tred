#pragma once

#include <memory>


namespace input
{


struct Table;
struct ConnectedUnits;


/** Represents a player.
The idea behind decoupling the active units and the player is that the unit
could be disconnected and swapped but the player should remain.
*/
struct Player
{
    Player();
    ~Player();

    void update_table(Table* table, float dt) const;
    [[nodiscard]] bool is_connected() const;
    [[nodiscard]] bool is_any_connection_considered_joystick() const;

    void update_connection_status();

    std::unique_ptr<ConnectedUnits> units;
};

}  // namespace input
