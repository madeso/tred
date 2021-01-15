#pragma once


#include "tred/input/axis.h"
#include "tred/input/axistype.h"


namespace input
{


struct KeyUnit
{
    virtual ~KeyUnit() = default;

    virtual void RegisterKey(int key) = 0;
    virtual float GetState(int key) = 0;
};


struct AxisUnit
{
    virtual ~AxisUnit() = default;

    virtual void RegisterAxis(AxisType type, int target, int axis) = 0;
    virtual float GetState(AxisType type, int target, int axis) = 0;
};


}
