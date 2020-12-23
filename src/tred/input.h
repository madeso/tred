#pragma once

#include <functional>
#include <map>
#include <memory>

#include "glm/glm.hpp"

#include "tred/types.h"


using FunctionCallback = std::function<void ()>;


struct Keybind
{
    int key;

    explicit Keybind(int key);
};

struct TwoButtonRange
{
    TwoButtonRange();
};

struct InputImpl;

enum class Range : u64 {};

struct Input
{
    Input();
    ~Input();

    void OnMouseMotion(const glm::vec2& motion);
    void OnMouseButton(int button, bool down);
    void OnKeyboard(int key, bool down);

    void AddFunction(const Keybind& bind, FunctionCallback&& function);

    Range AddRange(const Keybind& negative, const Keybind& positive);

    float Get(Range range);

    std::unique_ptr<InputImpl> impl;
};
