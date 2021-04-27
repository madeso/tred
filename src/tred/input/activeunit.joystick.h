#pragma once

#include <vector>
#include <map>

#include "tred/input/axis.h"
#include "tred/input/hataxis.h"
#include "tred/input/activeunit.h"
#include "tred/input/bind.h"
#include "tred/input/platform.h"


namespace input
{


struct table;
struct input_director;

struct joystick_active_unit;


namespace impl
{
    struct joystick_key_unit : public key_unit
    {
        joystick_active_unit* parent = nullptr;

        void register_key(int key) override;
        float get_state(int key) override;
    };

    struct joystick_axis_unit : public axis_unit
    {
        joystick_axis_unit(bool is_relative);

        bool is_relative;

        joystick_active_unit* parent;

        void register_axis(axis_type type, int target, int axis) override;
        float get_state(axis_type type, int target, int axis, float dt) override;
    };
}


struct joystick_active_unit : public active_unit
{
    joystick_active_unit(joystick_id joystick, input_director* director);
    ~joystick_active_unit() override;

    key_unit* get_key_unit() override;
    axis_unit* get_relative_axis_unit() override;
    axis_unit* get_absolute_axis_unit() override;

    bool is_considered_joystick() override;
    bool is_delete_scheduled() override;

    void on_axis(int axis, float state);
    void on_button(int button, float state);
    void on_hat(const hat_and_xy_axis& hatAxis, float state);
    void on_ball(const hat_and_xy_axis& hatAxis, float state);

    joystick_id joystick;
    input_director* director;
    bool scheduled_delete;

    bind_map<int> axes;
    bind_map<int> buttons;
    bind_map<hat_and_xy_axis> hats;
    bind_map<hat_and_xy_axis> balls;

    impl::joystick_key_unit key_unit;
    impl::joystick_axis_unit relative_axis_unit;
    impl::joystick_axis_unit absolute_axis_unit;
};


}
