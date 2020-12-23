#include "tred/input.h"


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

    explicit Callback(FunctionCallback&& c)
        : callback(std::move(c))
    {
    }

    void OnChange(bool new_state) override
    {
        if(!new_state) { return; }
        callback();
    }
};


struct Toggle : public Function
{
    bool state = false;

    void OnChange(bool new_state) override
    {
        state = new_state;
    }
};


}


Keybind::Keybind(int k)
    : key(k)
{
}


struct InputImpl
{
    std::map<int, std::unique_ptr<input::Function>> map;

    void AddFunction(const Keybind& bind, std::unique_ptr<input::Function>&& function)
    {
        map[bind.key] = std::move(function);
    }
};


Input::Input() = default;
Input::~Input() = default;

void Input::OnMouseMotion(const glm::vec2&)
{
}


void Input::OnMouseButton(int, bool)
{
}


void Input::OnKeyboard(int key, bool down)
{
    auto found = impl->map.find(key);
    if(found == impl->map.end()) { return; }

    auto& func = found->second;
    func->OnChange(down);
}


void Input::AddFunction(const Keybind& bind, FunctionCallback&& function)
{
    impl->AddFunction(bind, std::make_unique<input::Callback>(std::move(function)));
}


