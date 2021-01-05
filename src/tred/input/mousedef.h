#pragma once

#include <vector>

#include "tred/input/unitdef.h"
#include "tred/input/axis.h"
#include "tred/input/bind.h"
#include "tred/input/key.h"


namespace input::config
{
struct MouseDef;
}


namespace input
{


struct InputActionMap;


struct MouseDef : public UnitDef
{
    MouseDef(const config::MouseDef& data, ConverterDef* converter);

    std::unique_ptr<ActiveUnit> Create(InputDirector* director, const UnitSetup& setup, Converter* converter) override;

    std::vector<BindDef<Axis>> axes;
    std::vector<BindDef<Axis>> wheels;
    std::vector<BindDef<MouseButton>> keys;
};


}  // namespace input
