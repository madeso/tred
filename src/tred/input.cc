#include "tred/input.h"

#include <vector>

#include "tred/handle.h"


using RangeHandle = HandleFunctions64<Range>;


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


struct Range
{
    virtual ~Range() = default;
    virtual float Get() = 0;
};


struct TwoButtonRange : public Range
{
    Toggle* positive;
    Toggle* negative;

    TwoButtonRange(Toggle* p, Toggle* n) : positive(p), negative(n) {}

    float Get() override
    {
        float state = 0.0f;
        if(positive->state) { state += 1; }
        if(negative->state) { state -= 1; }
        return state;
    }
};


} // namespace input


Keybind::Keybind(int k)
    : key(k)
{
}


struct InputImpl
{
    std::map<int, std::unique_ptr<input::Function>> map;
    std::vector<std::unique_ptr<input::Range>> ranges;

    void AddFunction(const Keybind& bind, std::unique_ptr<input::Function>&& function)
    {
        map[bind.key] = std::move(function);
    }
};


Input::Input()
    : impl(new InputImpl{})
{
}


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


Range Input::AddRange(const Keybind& negative, const Keybind& positive)
{
    auto positive_range = std::make_unique<input::Toggle>();
    auto negative_range = std::make_unique<input::Toggle>();

    impl->ranges.emplace_back(std::make_unique<input::TwoButtonRange>(positive_range.get(), negative_range.get()));
    impl->AddFunction(positive, std::move(positive_range));
    impl->AddFunction(negative, std::move(negative_range));

    return RangeHandle::Compress(static_cast<RangeHandle::TId>(impl->ranges.size()-1), 0);
}

float Input::Get(Range range)
{
    const auto index = RangeHandle::GetId(range);
    return impl->ranges[index]->Get();
}

