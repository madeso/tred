#pragma once

#include <memory>


namespace input
{


struct InputDirector;
struct ActiveUnit;


struct UnitDef
{
    virtual ~UnitDef();

    virtual std::unique_ptr<ActiveUnit> Create(InputDirector* director) = 0;
};

}  // namespace input
