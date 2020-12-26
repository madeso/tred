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
    InputDirector* director;
    std::map<int, BindData> axes;
    std::map<int, BindData> buttons;
    std::map<HatAxis, BindData> hats;
};


}  // namespace input
