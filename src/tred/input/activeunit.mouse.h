#pragma once

#include <vector>
#include <map>

#include "tred/input/axis.h"
#include "tred/input/key.h"
#include "tred/input/activeunit.h"
#include "tred/input/bind.h"


namespace input
{


struct Table;
struct Director;

struct MouseActiveUnit;


namespace impl
{
    struct MouseKeyUnit : public KeyUnit
    {
        MouseActiveUnit* parent = nullptr;

        void register_key(int key) override;
        float get_state(int key) override;
    };

    struct RelativeMouseAxisUnit : public AxisUnit
    {
        MouseActiveUnit* parent = nullptr;

        void register_axis(AxisType type, int target, int axis) override;
        float get_state(AxisType type, int target, int axis, float) override;
    };

    struct AbsoluteMouseAxisUnit : public AxisUnit
    {
        MouseActiveUnit* parent = nullptr;

        void register_axis(AxisType type, int target, int axis) override;
        float get_state(AxisType type, int target, int axis, float) override;
    };
}


struct MouseActiveUnit : public ActiveUnit
{
    MouseActiveUnit(Director* director);
    ~MouseActiveUnit() override;

    KeyUnit* get_key_unit() override;
    AxisUnit* get_relative_axis_unit() override;
    AxisUnit* get_absolute_axis_unit() override;

    bool is_considered_joystick() override;
    bool is_delete_scheduled() override;

    void on_axis(const Axis2& axis, float relative_state, float absolute_state);
    void on_wheel(const Axis2& axis, float state);
    void on_button(MouseButton button, float state);

    Director* director;
    BindMap<Axis2> relative_axes;
    BindMap<Axis2> absolute_axes;
    BindMap<Axis2> wheels;
    BindMap<MouseButton> buttons;

    impl::MouseKeyUnit key_unit;
    impl::RelativeMouseAxisUnit relative_axis_unit;
    impl::AbsoluteMouseAxisUnit absolute_axis_unit;
};


}
