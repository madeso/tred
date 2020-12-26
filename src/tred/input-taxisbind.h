#pragma once

#include <memory>
#include <cassert>

#include "tred/input-bind.h"


namespace input
{

template <typename T>
struct TAxisBind
{
    TAxisBind(T t, std::shared_ptr<Bind> b, bool i, float s)
        : type(t)
        , bind(b)
        , invert(i)
        , scale(s)
    {
        assert(bind);
    }

    T type;
    std::shared_ptr<Bind> bind;
    bool invert;
    float scale;
};


}  // namespace input
