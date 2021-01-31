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
struct InputDirector;

struct JoystickActiveUnit;


namespace impl
{
    struct JoystickKeyUnit : public KeyUnit
    {
        JoystickActiveUnit* parent;

        void RegisterKey(int key) override;
        float GetState(int key) override;
    };

    struct JoystickAxisUnit : public AxisUnit
    {
        JoystickAxisUnit(bool is_relative);

        bool is_relative;

        JoystickActiveUnit* parent;

        void RegisterAxis(AxisType type, int target, int axis) override;
        float GetState(AxisType type, int target, int axis, float dt) override;
    };
}


struct JoystickActiveUnit : public ActiveUnit
{
    JoystickActiveUnit(JoystickId joystick, InputDirector* director);
    ~JoystickActiveUnit();

    KeyUnit* GetKeyUnit() override;
    AxisUnit* GetRelativeAxisUnit() override;
    AxisUnit* GetAbsoluteAxisUnit() override;

    bool IsConsideredJoystick() override;
    bool IsDeleteSheduled() override;

    void OnAxis(int axis, float state);
    void OnButton(int button, float state);
    void OnHat(const HatAxis& hatAxis, float state);
    void OnBall(const HatAxis& hatAxis, float state);

    JoystickId joystick;
    InputDirector* director;
    bool sheduled_delete;

    BindMap<int> axes;
    BindMap<int> buttons;
    BindMap<HatAxis> hats;
    BindMap<HatAxis> balls;

    impl::JoystickKeyUnit key_unit;
    impl::JoystickAxisUnit relative_axis_unit;
    impl::JoystickAxisUnit absolute_axis_unit;
};


}  // namespace input
