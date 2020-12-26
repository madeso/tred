// Euphoria - Copyright (c) Gustav

/** @file
Classes for input handling.
 */

#ifndef EUPHORIA_INPUT_ACTIVEAXISTORANGE_H_
#define EUPHORIA_INPUT_ACTIVEAXISTORANGE_H_

#include "tred/input-active.h"



namespace input {

struct InputAction;
struct Bind;

/** A input active range.
 */
struct ActiveAxisToRange : public InputActive {
 public:
  /** Constructor.
  @param action the action this active belongs to
   */
  ActiveAxisToRange(InputAction* action, Bind* axis);

  void Update(float dt);

 private:
  Bind* axis_;
};

}  // namespace input



#endif  // EUPHORIA_INPUT_ACTIVEAXISTORANGE_H_
