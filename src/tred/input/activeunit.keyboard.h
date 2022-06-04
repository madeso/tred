#pragma once


#include <map>
#include <memory>

#include "tred/input/activeunit.h"
#include "tred/input/bind.h"
#include "tred/input/key.h"
#include "tred/input/bindunit.h"


namespace input
{

struct AxisKey;
struct Director;

struct KeyboardActiveUnit;

namespace impl
{
    struct KeyboardKeyUnit : public KeyUnit
    {
        KeyboardActiveUnit* parent = nullptr;

        void register_key(int key) override;
        float get_state(int key) override;
    };
}


struct KeyboardActiveUnit : public ActiveUnit
{
    KeyboardActiveUnit(Director* director);
    ~KeyboardActiveUnit() override;

    KeyUnit* get_key_unit() override;
    AxisUnit* get_relative_axis_unit() override;
    AxisUnit* get_absolute_axis_unit() override;

    bool is_considered_joystick() override;
    bool is_delete_scheduled() override;

    void on_key(const KeyboardKey& key, bool state);

    Director* director;
    BindMap<KeyboardKey> keys;
    impl::KeyboardKeyUnit key_unit;
};


}
