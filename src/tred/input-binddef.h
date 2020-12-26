#pragma once

#include <string>
#include <cassert>

#include "tred/input-config.h"


namespace input
{


template <typename Type>
struct BindDef
{
    template <typename D>
    BindDef(const std::string& i, const Type t, const D& d)
        : id(i)
        , type(t)
        , invert(d.invert)
        , scale(d.scale)
    {
    }

    std::string id;
    Type type;
    bool invert;
    float scale;
};


}  // namespace input
