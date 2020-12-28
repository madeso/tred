#pragma once

#include <vector>
#include <map>

#include "tred/input-axis.h"
#include "tred/input-key.h"
#include "tred/input-activeunit.h"
#include "tred/input-tbind.h"


struct Table;


namespace input
{

struct InputDirector;
struct BindData;


struct MouseActiveUnit : public ActiveUnit
{
    MouseActiveUnit
    (
        const std::vector<std::shared_ptr<TBind<Axis>>>& axis,
        const std::vector<std::shared_ptr<TBind<MouseButton>>>& buttons,
        InputDirector* director
    );
    ~MouseActiveUnit();

    void OnAxis(const Axis& key, float state);
    void OnButton(MouseButton key, float state);

    InputDirector* director;
    const std::map<Axis, BindData> actions;
    const std::map<MouseButton, BindData> buttons;
};


}  // namespace input
