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
struct input_director;

struct keyboard_active_unit;

namespace impl
{
    struct keyboard_key_unit : public key_unit
    {
        keyboard_active_unit* parent = nullptr;

        void register_key(int key) override;
        float get_state(int key) override;
    };
}


struct keyboard_active_unit : public active_unit
{
    keyboard_active_unit(input_director* director);
    ~keyboard_active_unit() override;

    key_unit* get_key_unit() override;
    axis_unit* get_relative_axis_unit() override;
    axis_unit* get_absolute_axis_unit() override;

    bool is_considered_joystick() override;
    bool is_delete_scheduled() override;

    void on_key(const keyboard_key& key, bool state);

    input_director* director;
    bind_map<keyboard_key> keys;
    impl::keyboard_key_unit key_unit;
};


}
