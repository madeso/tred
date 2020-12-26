/** @file
Classes for input handling.
 */

#pragma once


struct Table;

namespace input {

struct InputAction;

/** Represents the accumulated input to be sent to the game logic.
 */
struct InputActive {
  /** Constructor.
  @param action the action this active belongs to
   */
  explicit InputActive(InputAction* action);

  /** Get the current value of the input.
  @returns the state
   */
  float state() const;

  /** Set the value of the input.
  @param state the new value
   */
  void set_state(float state);

  /** Gets the action this active belongs to.
  @returns the action this active belongs to
   */
  const InputAction& action() const;

  InputAction* action_;
  float state_;
};

}  // namespace input




