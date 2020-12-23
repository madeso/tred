#include "tred/input.h"


Keybind::Keybind(int k) : key(k)
{
}


void Input::OnMouseMotion(const glm::vec2&)
{
}


void Input::OnMouseButton(int, bool)
{
}


void Input::OnKeyboard(int key, bool down)
{
    if(down == false) { return; }

    auto found = map.find(key);
    if(found == map.end()) { return; }

    auto& func = found->second;
    (*func)();
}


void Input::AddFunction(const Keybind& bind, FunctionCallback&& function)
{
    map[bind.key] = std::make_unique<FunctionCallback>(std::move(function));
}

