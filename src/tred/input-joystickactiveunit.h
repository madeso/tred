#pragma once

#include <vector>
#include <map>

#include "tred/input-axis.h"
#include "tred/input-hataxis.h"
#include "tred/input-activeunit.h"
#include "tred/input-tbind.h"


struct Table;

namespace input
{
struct BindData;
struct InputDirector;

struct JoystickActiveUnit : public ActiveUnit
{
    JoystickActiveUnit
    (
        int joystick, InputDirector* director,
        const std::vector<std::shared_ptr<TBind<int>>>& axis,
        const std::vector<std::shared_ptr<TBind<int>>>& buttons,
        const std::vector<std::shared_ptr<TBind<HatAxis>>>& hats
    );
    ~JoystickActiveUnit();

    void OnAxis(int axis, float state);
    void OnButton(int button, float state);
    void OnHat(const HatAxis& hatAxis, float state);

    // int joystick_;
    InputDirector* director;
    std::map<int, BindData> axes;
    std::map<int, BindData> buttons;
    std::map<HatAxis, BindData> hats;
};


}  // namespace input
