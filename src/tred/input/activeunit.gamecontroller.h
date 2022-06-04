#pragma once


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
struct Director;

struct GamecontrollerActiveUnit;


namespace impl
{
    struct GamecontrollerKeyUnit : public KeyUnit
    {
        GamecontrollerActiveUnit* parent = nullptr;

        void register_key(int key) override;
        float get_state(int key) override;
    };

    struct GamecontrollerAxisUnit : public AxisUnit
    {
        GamecontrollerAxisUnit(bool is_relative);

        bool is_relative;

        GamecontrollerActiveUnit* parent = nullptr;

        void register_axis(AxisType type, int target, int axis) override;
        float get_state(AxisType type, int target, int axis, float dt) override;
    };
}


struct GamecontrollerActiveUnit : public ActiveUnit
{
    GamecontrollerActiveUnit(JoystickId j, Director* director);
    ~GamecontrollerActiveUnit();

    KeyUnit* get_key_unit() override;
    AxisUnit* get_relative_axis_unit() override;
    AxisUnit* get_absolute_axis_unit() override;

    bool is_considered_joystick() override;
    bool is_delete_scheduled() override;

    void on_axis(GamecontrollerAxis axis, float state);
    void on_button(GamecontrollerButton button, float state);

    JoystickId joystick;
    Director* director;
    bool scheduled_delete;

    BindMap<GamecontrollerAxis> axes;
    BindMap<GamecontrollerButton> buttons;

    impl::GamecontrollerKeyUnit key_unit;
    impl::GamecontrollerAxisUnit relative_axis_unit;
    impl::GamecontrollerAxisUnit absolute_axis_unit;
};


}  // namespace input
