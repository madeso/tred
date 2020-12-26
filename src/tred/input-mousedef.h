#pragma once

#include <vector>

#include "tred/input-unitdef.h"
#include "tred/input-axis.h"
#include "tred/input-binddef.h"
#include "tred/input-key.h"


namespace config
{
struct MouseDef;
}

namespace input
{


struct InputActionMap;

struct MouseDef : public UnitDef
{
    MouseDef(const config::MouseDef& data, const InputActionMap& map);
    std::shared_ptr<ActiveUnit> Create(InputDirector* director, BindMap* map) override;

    std::vector<BindDef<Axis>> axis;
    std::vector<BindDef<MouseButton>> keys;
};


}  // namespace input
