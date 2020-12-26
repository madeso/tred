#pragma once

#include <vector>
#include <map>

#include "tred/input-axis.h"
#include "tred/input-key.h"
#include "tred/input-activeunit.h"
#include "tred/input-taxisbind.h"
#include "tred/input-trangebind.h"


struct Table;


namespace input
{

struct InputDirector;
struct BindData;


struct MouseActiveUnit : public ActiveUnit
{
    MouseActiveUnit
    (
        const std::vector<std::shared_ptr<TAxisBind<Axis>>>& axis,
        const std::vector<std::shared_ptr<TRangeBind<MouseButton>>>& buttons,
        InputDirector* director
    );
    ~MouseActiveUnit();

    void OnAxis(const Axis& key, float state);
    void OnButton(MouseButton key, float state);

    void Rumble() override;

    InputDirector* director;
    const std::map<Axis, BindData> actions;
    const std::map<MouseButton, BindData> buttons;
};


}  // namespace input
