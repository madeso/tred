#pragma once

#include <vector>
#include <map>

#include "tred/input/key.h"
#include "tred/input/axis.h"
#include "tred/input/hataxis.h"
#include "tred/input/activeunit.h"
#include "tred/input/bind.h"
#include "tred/input/platform.h"


namespace input
{


struct table;
struct input_director;

struct gamecontroller_active_unit;


namespace impl
{
    struct gamecontroller_key_unit : public key_unit
    {
        gamecontroller_active_unit* parent = nullptr;

        void register_key(int key) override;
        float get_state(int key) override;
    };

    struct gamecontroller_axis_unit : public axis_unit
    {
        gamecontroller_axis_unit(bool is_relative);

        bool is_relative;

        gamecontroller_active_unit* parent = nullptr;

        void register_axis(axis_type type, int target, int axis) override;
        float get_state(axis_type type, int target, int axis, float dt) override;
    };
}


struct gamecontroller_active_unit : public active_unit
{
    gamecontroller_active_unit(joystick_id Gamecontroller, input_director* director);
    ~gamecontroller_active_unit();

    key_unit* get_key_unit() override;
    axis_unit* get_relative_axis_unit() override;
    axis_unit* get_absolute_axis_unit() override;

    bool is_considered_joystick() override;
    bool is_delete_scheduled() override;

    void on_axis(gamecontroller_axis axis, float state);
    void on_button(gamecontroller_button button, float state);

    joystick_id joystick;
    input_director* director;
    bool scheduled_delete;

    bind_map<gamecontroller_axis> axes;
    bind_map<gamecontroller_button> buttons;

    impl::gamecontroller_key_unit key_unit;
    impl::gamecontroller_axis_unit relative_axis_unit;
    impl::gamecontroller_axis_unit absolute_axis_unit;
};


}  // namespace input
