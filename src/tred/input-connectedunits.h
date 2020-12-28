#pragma once

#include <vector>
#include <memory>


#include "input-bind.h"



namespace input
{


struct Table;
struct ActiveUnit;


struct ConnectedUnits
{
    ConnectedUnits(const Converter& converter);
    
    void UpdateTable(Table* table);
    void Update(float dt);
    void Add(std::shared_ptr<ActiveUnit> unit);
    bool IsEmpty() const;

    Converter converter;
    std::vector<std::shared_ptr<ActiveUnit>> units;
};

}  // namespace input
