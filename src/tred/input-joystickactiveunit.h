#pragma once

#include <vector>
#include <map>

#include "tred/input-axis.h"
#include "tred/input-hataxis.h"
#include "tred/input-activeunit.h"
#include "tred/input-taxisbind.h"
#include "tred/input-trangebind.h"


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
        const std::vector<std::shared_ptr<TAxisBind<int>>>& axis,
        const std::vector<std::shared_ptr<TRangeBind<int>>>& buttons,
        const std::vector<std::shared_ptr<TAxisBind<HatAxis>>>& hats
    );
    ~JoystickActiveUnit();

    void OnAxis(int axis, float state);
    void OnButton(int button, float state);
    void OnHat(const HatAxis& hatAxis, float state);
    
    void Rumble() override;

    // int joystick_;
    InputDirector* director_;
    std::map<int, BindData> axis_;
    std::map<int, BindData> buttons_;
    std::map<HatAxis, BindData> hats_;
};


}  // namespace input
