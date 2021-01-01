#pragma once

#include <vector>
#include <map>

#include "tred/input/axis.h"
#include "tred/input/hataxis.h"
#include "tred/input/activeunit.h"
#include "tred/input/bind.h"



namespace input
{


struct Table;
struct InputDirector;


struct JoystickActiveUnit : public ActiveUnit
{
    JoystickActiveUnit
    (
        int joystick, InputDirector* director, Converter* converter,
        const std::vector<BindDef<int>>& axis,
        const std::vector<BindDef<int>>& buttons,
        const std::vector<BindDef<HatAxis>>& hats
    );
    ~JoystickActiveUnit();

    void OnAxis(int axis, float state);
    void OnButton(int button, float state);
    void OnHat(const HatAxis& hatAxis, float state);

    // int joystick_;
    InputDirector* director;
    BindMap<int> axes;
    BindMap<int> buttons;
    BindMap<HatAxis> hats;
};


}  // namespace input
