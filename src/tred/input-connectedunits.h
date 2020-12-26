#pragma once

#include <vector>
#include <memory>


struct Table;

namespace input
{


struct ActiveUnit;
struct ActiveList;

struct ConnectedUnits
{
    explicit ConnectedUnits(std::shared_ptr<ActiveList> actives);

    void UpdateTable(Table* table);
    void Update(float dt);
    void Add(std::shared_ptr<ActiveUnit> unit);
    bool IsEmpty() const;

    std::vector<std::shared_ptr<ActiveUnit>> units;
    std::shared_ptr<ActiveList> actives;
};

}  // namespace input
