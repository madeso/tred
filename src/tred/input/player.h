#pragma once

#include <memory>


namespace input
{


struct table;
struct connected_units;


/** Represents a player.
The idea behind decoupling the active units and the player is that the unit
could be disconnected and swapped but the player should remain.
*/
struct player
{
    player();
    ~player();

    void update_table(table* table, float dt) const;
    [[nodiscard]] bool is_connected() const;
    [[nodiscard]] bool is_any_connection_considered_joystick() const;

    void update_connection_status();

    std::unique_ptr<connected_units> units;
};

}  // namespace input
