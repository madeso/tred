#pragma once

#include <vector>
#include <map>
#include <memory>

#include "tred/input/activeunit.h"
#include "tred/input/bind.h"
#include "tred/input/key.h"


namespace input
{

struct AxisKey;
struct InputDirector;


struct KeyboardActiveUnit : public ActiveUnit
{
    KeyboardActiveUnit(InputDirector* director, Converter* converter, const std::vector<BindDef<Key>>& binds);
    ~KeyboardActiveUnit();

    void OnKey(const Key& key, bool state);

    InputDirector* director;
    BindMap<Key> keys;
};


}  // namespace input