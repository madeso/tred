#pragma once

#include <vector>
#include <map>
#include <memory>

#include "tred/input/activeunit.h"
#include "tred/input/bind.h"
#include "tred/input/key.h"
#include "tred/input/bindunit.h"


namespace input
{

struct AxisKey;
struct InputDirector;

struct KeyboardActiveUnit;

namespace impl
{
    struct KeyboardKeyUnit : public KeyUnit
    {
        KeyboardActiveUnit* parent;

        void RegisterKey(int key);
        float GetState(int key);
    };
}


struct KeyboardActiveUnit : public ActiveUnit
{
    KeyboardActiveUnit(InputDirector* director);
    ~KeyboardActiveUnit();

    KeyUnit* GetKeyUnit() override;
    AxisUnit* GetRelativeAxisUnit() override;
    AxisUnit* GetAbsoluteAxisUnit() override;

    bool IsConsideredJoystick() override;
    bool IsDeleteSheduled() override;

    void OnKey(const Key& key, bool state);

    InputDirector* director;
    BindMap<Key> keys;
    impl::KeyboardKeyUnit key_unit;
};


}  // namespace input
