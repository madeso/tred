#pragma once

#include <vector>
#include <map>
#include <memory>

#include "tred/input-activeunit.h"
#include "tred/input-trangebind.h"
#include "tred/input-key.h"


namespace input
{

struct AxisKey;
struct InputDirector;
struct BindData;

struct KeyboardActiveUnit : public ActiveUnit
{
    KeyboardActiveUnit(const std::vector<std::shared_ptr<TRangeBind<Key>>>& binds, InputDirector* director);
    ~KeyboardActiveUnit();

    void OnKey(const Key& key, bool state);

    InputDirector* director;
    const std::map<Key, BindData> actions;
};

}  // namespace input
