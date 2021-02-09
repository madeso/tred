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


struct Table;
struct InputDirector;

struct GamecontrollerActiveUnit;


namespace impl
{
    struct GamecontrollerKeyUnit : public KeyUnit
    {
        GamecontrollerActiveUnit* parent;

        void RegisterKey(int key) override;
        float GetState(int key) override;
    };

    struct GamecontrollerAxisUnit : public AxisUnit
    {
        GamecontrollerAxisUnit(bool is_relative);

        bool is_relative;

        GamecontrollerActiveUnit* parent;

        void RegisterAxis(AxisType type, int target, int axis) override;
        float GetState(AxisType type, int target, int axis, float dt) override;
    };
}


struct GamecontrollerActiveUnit : public ActiveUnit
{
    GamecontrollerActiveUnit(JoystickId Gamecontroller, InputDirector* director);
    ~GamecontrollerActiveUnit();

    KeyUnit* GetKeyUnit() override;
    AxisUnit* GetRelativeAxisUnit() override;
    AxisUnit* GetAbsoluteAxisUnit() override;

    bool IsConsideredJoystick() override;
    bool IsDeleteSheduled() override;

    void OnAxis(GamecontrollerAxis axis, float state);
    void OnButton(GamecontrollerButton button, float state);

    JoystickId joystick;
    InputDirector* director;
    bool sheduled_delete;

    BindMap<GamecontrollerAxis> axes;
    BindMap<GamecontrollerButton> buttons;

    impl::GamecontrollerKeyUnit key_unit;
    impl::GamecontrollerAxisUnit relative_axis_unit;
    impl::GamecontrollerAxisUnit absolute_axis_unit;
};


}  // namespace input
