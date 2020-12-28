#pragma once

#include <vector>
#include <map>

#include "tred/input-axis.h"
#include "tred/input-hataxis.h"
#include "tred/input-activeunit.h"
#include "tred/input-bind.h"


struct Table;

namespace input
{
struct InputDirector;

struct JoystickActiveUnit : public ActiveUnit
{
    JoystickActiveUnit
    (
        int joystick, InputDirector* director,
        const std::vector<BindDef<int>>& axis,
        const std::vector<BindDef<int>>& buttons,
        const std::vector<BindDef<HatAxis>>& hats
    );
    ~JoystickActiveUnit();

    void Recieve(ValueReciever* reciever) override;

    void OnAxis(int axis, float state);
    void OnButton(int button, float state);
    void OnHat(const HatAxis& hatAxis, float state);

    // int joystick_;
    InputDirector* director;
    std::map<int, Bind> axes;
    std::map<int, Bind> buttons;
    std::map<HatAxis, Bind> hats;
};


}  // namespace input
