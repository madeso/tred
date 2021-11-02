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


struct Table;
struct Director;

struct JoystickActiveUnit;


namespace impl
{
    struct JoystickKeyUnit : public KeyUnit
    {
        JoystickActiveUnit* parent = nullptr;

        void register_key(int key) override;
        float get_state(int key) override;
    };

    struct JoystickAxisUnit : public AxisUnit
    {
        JoystickAxisUnit(bool is_relative);

        bool is_relative;

        JoystickActiveUnit* parent;

        void register_axis(AxisType type, int target, int axis) override;
        float get_state(AxisType type, int target, int axis, float dt) override;
    };
}


struct JoystickActiveUnit : public ActiveUnit
{
    JoystickActiveUnit(JoystickId joystick, Director* director);
    ~JoystickActiveUnit() override;

    KeyUnit* get_key_unit() override;
    AxisUnit* get_relative_axis_unit() override;
    AxisUnit* get_absolute_axis_unit() override;

    bool is_considered_joystick() override;
    bool is_delete_scheduled() override;

    void on_axis(int axis, float state);
    void on_button(int button, float state);
    void on_hat(const HatAndAxis2& hatAxis, float state);
    void on_ball(const HatAndAxis2& hatAxis, float state);

    JoystickId joystick;
    Director* director;
    bool scheduled_delete;

    BindMap<int> axes;
    BindMap<int> buttons;
    BindMap<HatAndAxis2> hats;
    BindMap<HatAndAxis2> balls;

    impl::JoystickKeyUnit key_unit;
    impl::JoystickAxisUnit relative_axis_unit;
    impl::JoystickAxisUnit absolute_axis_unit;
};


}
