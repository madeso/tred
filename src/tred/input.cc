#include "tred/input.h"


namespace input
{


Callback::Callback(FunctionCallback&& c)
    : callback(std::move(c))
{
}


void Callback::OnChange(bool new_state)
{
    if(!new_state) { return; }
    callback();
}


void Toggle::OnChange(bool new_state)
{
    state = new_state;
}


}


Keybind::Keybind(int k)
    : key(k)
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
    auto found = map.find(key);
    if(found == map.end()) { return; }

    auto& func = found->second;
    func->OnChange(down);
}


void Input::AddFunction(const Keybind& bind, FunctionCallback&& function)
{
    AddFunction(bind, std::make_unique<input::Callback>(std::move(function)));
}

void Input::AddFunction(const Keybind& bind, std::unique_ptr<input::Function>&& function)
{
    map[bind.key] = std::move(function);
}

