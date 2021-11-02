#pragma once


#include "tred/input/axis.h"
#include "tred/input/axistype.h"


namespace input
{


struct KeyUnit
{
    virtual ~KeyUnit() = default;

    virtual void register_key(int key) = 0;
    virtual float get_state(int key) = 0;
};


struct AxisUnit
{
    virtual ~AxisUnit() = default;

    virtual void register_axis(AxisType type, int target, int axis) = 0;
    virtual float get_state(AxisType type, int target, int axis, float dt) = 0;
};


}
