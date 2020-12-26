/** @file
Classes for input handling.
 */

#pragma once
#include "tred/input-active.h"


namespace input
{
struct InputAction;
struct Bind;

/** A input active range.
 */
struct ActiveAxisToRange : public InputActive
{
    /** Constructor.
  @param action the action this active belongs to
   */
    ActiveAxisToRange(InputAction* action, Bind* axis);

    void Update(float dt);

    Bind* axis_;
};

}  // namespace input
