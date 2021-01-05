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


struct JoystickActiveUnit : public ActiveUnit
{
    JoystickActiveUnit
    (
        JoystickId joystick, InputDirector* director, Converter* converter,
        const std::vector<BindDef<int>>& axis,
        const std::vector<BindDef<int>>& buttons,
        const std::vector<BindDef<HatAxis>>& hats,
        const std::vector<BindDef<HatAxis>>& balls
    );
    ~JoystickActiveUnit();

    void OnAxis(int axis, float state);
    void OnButton(int button, float state);
    void OnHat(const HatAxis& hatAxis, float state);
    void OnBall(const HatAxis& hatAxis, float state);

    JoystickId joystick;
    InputDirector* director;
    BindMap<int> axes;
    BindMap<int> buttons;
    BindMap<HatAxis> hats;
    BindMap<HatAxis> balls;
};


}  // namespace input
