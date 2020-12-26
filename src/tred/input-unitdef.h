#pragma once

#include <memory>


namespace input
{

struct InputDirector;
struct ActiveUnit;
struct BindMap;


struct UnitDef
{
    virtual ~UnitDef();

    virtual std::shared_ptr<ActiveUnit> Create(InputDirector* director, BindMap* map) = 0;
};

}  // namespace input
