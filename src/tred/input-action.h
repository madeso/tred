#pragma once

#include <string>

#include "tred/input-range.h"


struct Table;

namespace input
{

struct GlobalToggle;

struct InputAction
{
    InputAction(const std::string& name, const std::string& scriptvarname, Range range, bool global);
    ~InputAction();

    const std::string& scriptvarname() const;
    Range range() const;
    const std::string& name() const;
    bool global() const;
    GlobalToggle* toggle();

    void Setup(GlobalToggle* toggle);
    void ClearToggle(GlobalToggle* toggle);

    std::string name_;
    std::string scriptvarname_;
    Range range_;
    bool global_;
    GlobalToggle* toggle_;
};

}  // namespace input
