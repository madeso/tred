#pragma once

#include <vector>
#include <map>

#include "tred/input-axis.h"
#include "tred/input-key.h"
#include "tred/input-activeunit.h"
#include "tred/input-bind.h"


struct Table;


namespace input
{

struct InputDirector;


struct MouseActiveUnit : public ActiveUnit
{
    MouseActiveUnit
    (
        InputDirector* director,
        const std::vector<BindDef<Axis>>& axes,
        const std::vector<BindDef<MouseButton>>& buttons
    );
    ~MouseActiveUnit();

    void Recieve(ValueReciever* reciever) override;

    void OnAxis(const Axis& key, float state);
    void OnButton(MouseButton key, float state);

    InputDirector* director;
    std::map<Axis, Bind> axes;
    std::map<MouseButton, Bind> buttons;
};


}  // namespace input
