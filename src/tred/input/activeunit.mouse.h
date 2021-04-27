#pragma once

#include <vector>
#include <map>

#include "tred/input/axis.h"
#include "tred/input/key.h"
#include "tred/input/activeunit.h"
#include "tred/input/bind.h"


namespace input
{


struct table;
struct input_director;

struct mouse_active_unit;


namespace impl
{
    struct mouse_key_unit : public key_unit
    {
        mouse_active_unit* parent = nullptr;

        void register_key(int key) override;
        float get_state(int key) override;
    };

    struct relative_mouse_axis_unit : public axis_unit
    {
        mouse_active_unit* parent = nullptr;

        void register_axis(axis_type type, int target, int axis) override;
        float get_state(axis_type type, int target, int axis, float) override;
    };

    struct absolute_mouse_axis_unit : public axis_unit
    {
        mouse_active_unit* parent = nullptr;

        void register_axis(axis_type type, int target, int axis) override;
        float get_state(axis_type type, int target, int axis, float) override;
    };
}


struct mouse_active_unit : public active_unit
{
    mouse_active_unit(input_director* director);
    ~mouse_active_unit() override;

    key_unit* get_key_unit() override;
    axis_unit* get_relative_axis_unit() override;
    axis_unit* get_absolute_axis_unit() override;

    bool is_considered_joystick() override;
    bool is_delete_scheduled() override;

    void on_axis(const xy_axis& axis, float relative_state, float absolute_state);
    void on_wheel(const xy_axis& axis, float state);
    void on_button(mouse_button button, float state);

    input_director* director;
    bind_map<xy_axis> relative_axes;
    bind_map<xy_axis> absolute_axes;
    bind_map<xy_axis> wheels;
    bind_map<mouse_button> buttons;

    impl::mouse_key_unit key_unit;
    impl::relative_mouse_axis_unit relative_axis_unit;
    impl::absolute_mouse_axis_unit absolute_axis_unit;
};


}
