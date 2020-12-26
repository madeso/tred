#pragma once

#include "tred/input-activeunit.h"


namespace input
{


struct DummyActiveUnit : public ActiveUnit
{
    void Rumble() override;
};


}  // namespace input
