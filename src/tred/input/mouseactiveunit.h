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
struct InputDirector;

struct MouseActiveUnit;


namespace impl
{
    struct MouseKeyUnit : public KeyUnit
    {
        MouseActiveUnit* parent;

        void RegisterKey(int key) override;
        float GetState(int key) override;
    };

    struct RelativeMouseAxisUnit : public AxisUnit
    {
        MouseActiveUnit* parent;

        void RegisterAxis(AxisType type, int target, int axis) override;
        float GetState(AxisType type, int target, int axis) override;
    };

    struct AbsoluteMouseAxisUnit : public AxisUnit
    {
        MouseActiveUnit* parent;

        void RegisterAxis(AxisType type, int target, int axis) override;
        float GetState(AxisType type, int target, int axis) override;
    };
}


struct MouseActiveUnit : public ActiveUnit
{
    MouseActiveUnit(InputDirector* director);
    ~MouseActiveUnit();

    KeyUnit* GetKeyUnit() override;
    AxisUnit* GetRelativeAxisUnit() override;
    AxisUnit* GetAbsoluteAxisUnit() override;

    bool IsConsideredJoystick() override;
    bool IsDeleteSheduled() override;

    void OnAxis(const Axis& axis, float state);
    void OnWheel(const Axis& axis, float state);
    void OnButton(MouseButton button, float state);

    InputDirector* director;
    BindMap<Axis> axes;
    BindMap<Axis> wheels;
    BindMap<MouseButton> buttons;

    impl::MouseKeyUnit key_unit;
    impl::RelativeMouseAxisUnit relative_axis_unit;
    impl::AbsoluteMouseAxisUnit absolute_axis_unit;
};


}  // namespace input
