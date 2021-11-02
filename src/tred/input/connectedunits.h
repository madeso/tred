#pragma once

#include <vector>
#include <memory>


namespace input
{

struct Table;
struct ActiveUnit;
struct ActiveBind;


struct ConnectedUnits
{
    ConnectedUnits();
    ~ConnectedUnits();

    void update_table(Table* table, float dt);
    void add(std::unique_ptr<ActiveUnit>&& unit);
    void add(std::unique_ptr<ActiveBind>&& bind);
    [[nodiscard]] bool is_empty() const;

    bool is_any_connection_considered_joystick();
    bool is_delete_scheduled();

    ActiveUnit* get_unit(int index);

    std::vector<std::unique_ptr<ActiveUnit>> units;
    std::vector<std::unique_ptr<ActiveBind>> binds;
};

}
