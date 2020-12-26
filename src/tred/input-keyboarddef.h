#pragma once

#include <vector>
#include <memory>

#include "tred/input-key.h"
#include "tred/input-unitdef.h"
#include "tred/input-binddef.h"


namespace input
{

namespace config
{
    struct KeyboardDef;
}

struct InputActionMap;
struct ActiveUnit;
struct InputDirector;

struct KeyboardDef : public UnitDef
{
    KeyboardDef(const config::KeyboardDef& data, const InputActionMap& map);

    std::shared_ptr<ActiveUnit> Create(InputDirector* director, BindMap* map) override;

    std::vector<BindDef<Key>> keys_;
};

}  // namespace input
