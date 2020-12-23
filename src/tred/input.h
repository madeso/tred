#pragma once

#include <functional>
#include <map>
#include <memory>

#include "glm/glm.hpp"


using FunctionCallback = std::function<void ()>;

namespace input
{

struct Function
{
    virtual ~Function() = default;
    virtual void OnChange(bool new_state) = 0;
};

struct Callback : public Function
{
    FunctionCallback callback;

    explicit Callback(FunctionCallback&& callback);
    void OnChange(bool new_state) override;
};

struct Toggle : public Function
{
    bool state = false;

    void OnChange(bool new_state) override;
};

}

struct Keybind
{
    int key;

    explicit Keybind(int key);
};

struct TwoButtonRange
{
    TwoButtonRange();
};

struct Input
{
    void OnMouseMotion(const glm::vec2& motion);
    void OnMouseButton(int button, bool down);
    void OnKeyboard(int key, bool down);

    void AddFunction(const Keybind& bind, FunctionCallback&& function);

    void AddFunction(const Keybind& bind, std::unique_ptr<input::Function>&& function);

    std::map<int, std::unique_ptr<input::Function>> map;
};
