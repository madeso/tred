// Euphoria - Copyright (c) Gustav

/** @file
Classes for input handling.
 */

#ifndef EUPHORIA_INPUT_ACTIVERANGETOAXIS_H_
#define EUPHORIA_INPUT_ACTIVERANGETOAXIS_H_

#include "tred/input-active.h"



namespace input {

struct InputAction;
struct Bind;

/** A input active range.
 */
struct ActiveRangeToAxis : public InputActive {
 public:
  /** Constructor.
  @param action the action this active belongs to
   */
  ActiveRangeToAxis(InputAction* action, Bind* positive, Bind* negative);

  void Update(float dt);

 private:
  Bind* positive_;
  Bind* negative_;
};

}  // namespace input



#endif  // EUPHORIA_INPUT_ACTIVERANGETOAXIS_H_
