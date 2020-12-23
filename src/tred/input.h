#pragma once

#include <functional>
#include <map>
#include <memory>

#include "glm/glm.hpp"


using FunctionCallback = std::function<void ()>;


struct Keybind
{
    int key;

    explicit Keybind(int key);
};

struct Input
{
    void OnMouseMotion(const glm::vec2& motion);
    void OnMouseButton(int button, bool down);
    void OnKeyboard(int key, bool down);

    void AddFunction(const Keybind& bind, FunctionCallback&& function);

    std::map<int, std::unique_ptr<FunctionCallback>> map;
};
