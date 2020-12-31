#pragma once

#include <vector>
#include <memory>


#include "tred/input/bind.h"


namespace input
{


struct Table;
struct ActiveUnit;


struct ConnectedUnits
{
    ConnectedUnits(const ConverterDef& converter);
    ~ConnectedUnits();
    
    void UpdateTable(Table* table);
    void Update(float dt);
    void Add(std::unique_ptr<ActiveUnit>&& unit);
    bool IsEmpty() const;

    Converter converter;
    std::vector<std::unique_ptr<ActiveUnit>> units;
};

}  // namespace input
