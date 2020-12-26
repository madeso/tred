#pragma once

#include <memory>
#include <vector>


namespace input
{


struct InputAction;
struct Bind;

/** Represents the accumulated input to be sent to the game logic.
 */
struct GlobalToggle
{
    explicit GlobalToggle(std::shared_ptr<InputAction> action);
    ~GlobalToggle();

    void Update();
    void Add(Bind* bind);
    void Remove(Bind* bind);

    bool state() const;

    std::shared_ptr<InputAction> action_;
    bool state_;
    bool last_down_;
    Bind* current_state_holder_;
    std::vector<Bind*> binds_;
};

}  // namespace input
