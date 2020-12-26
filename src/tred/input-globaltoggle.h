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

    std::shared_ptr<InputAction> action;
    bool state;
    bool last_down;
    Bind* current_state_holder;
    std::vector<Bind*> binds;
};

}  // namespace input
