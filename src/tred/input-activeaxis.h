// Euphoria - Copyright (c) Gustav

/** @file
Classes for input handling.
 */

#ifndef EUPHORIA_INPUT_ACTIVEAXIS_H_
#define EUPHORIA_INPUT_ACTIVEAXIS_H_

#include "tred/input-active.h"



namespace input {

struct InputAction;
struct Bind;

/** A input active axis.
 */
struct ActiveAxis : public InputActive {
 public:
  /** Constructor.
  @param action the action this active belongs to
   */
  ActiveAxis(InputAction* action, Bind* axis);

  void Update(float dt);

 private:
  Bind* axis_;
};

}  // namespace input



#endif  // EUPHORIA_INPUT_ACTIVEAXIS_H_
