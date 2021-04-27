#pragma once


#include "tred/input/axis.h"
#include "tred/input/axistype.h"


namespace input
{


struct key_unit
{
    virtual ~key_unit() = default;

    virtual void register_key(int key) = 0;
    virtual float get_state(int key) = 0;
};


struct axis_unit
{
    virtual ~axis_unit() = default;

    virtual void register_axis(axis_type type, int target, int axis) = 0;
    virtual float get_state(axis_type type, int target, int axis, float dt) = 0;
};


}
