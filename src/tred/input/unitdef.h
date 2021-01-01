#pragma once

#include <memory>


namespace input
{


struct InputDirector;
struct ActiveUnit;
struct Converter;


struct UnitDef
{
    virtual ~UnitDef();

    virtual std::unique_ptr<ActiveUnit> Create(InputDirector* director, Converter* converter) = 0;
};

}  // namespace input
