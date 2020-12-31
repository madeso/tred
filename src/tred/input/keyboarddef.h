#pragma once

#include <vector>
#include <memory>

#include "tred/input/key.h"
#include "tred/input/unitdef.h"
#include "tred/input/bind.h"


namespace input::config
{
struct KeyboardDef;
}


namespace input
{


struct InputActionMap;
struct ActiveUnit;
struct InputDirector;

struct KeyboardDef : public UnitDef
{
    KeyboardDef(const config::KeyboardDef& data, ConverterDef* converter);

    std::unique_ptr<ActiveUnit> Create(InputDirector* director) override;

    std::vector<BindDef<Key>> keys;
};


}  // namespace input
