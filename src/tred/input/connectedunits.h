#pragma once

#include <vector>
#include <memory>


namespace input
{

struct table;
struct active_unit;
struct active_bind;


struct connected_units
{
    connected_units();
    ~connected_units();

    void update_table(table* table, float dt);
    void add(std::unique_ptr<active_unit>&& unit);
    void add(std::unique_ptr<active_bind>&& bind);
    [[nodiscard]] bool is_empty() const;

    bool is_any_connection_considered_joystick();
    bool is_delete_scheduled();

    active_unit* get_unit(int index);

    std::vector<std::unique_ptr<active_unit>> units;
    std::vector<std::unique_ptr<active_bind>> binds;
};

}
