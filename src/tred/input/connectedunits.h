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

    void UpdateTable(Table* table);
    void Add(std::unique_ptr<ActiveUnit>&& unit);
    void Add(std::unique_ptr<ActiveBind>&& bind);
    bool IsEmpty() const;

    bool IsAnyConnectionConsideredJoystick();
    bool IsDeleteSheduled();

    ActiveUnit* GetUnit(int index);

    std::vector<std::unique_ptr<ActiveUnit>> units;
    std::vector<std::unique_ptr<ActiveBind>> binds;
};

}  // namespace input
