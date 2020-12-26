/** @file
Classes for input handling.
 */

#pragma once
#include <memory>
#include <cassert>

#include "tred/input-active.h"



namespace input {

struct ActiveRange;
struct ActiveAxisToRange;

struct ActiveMasterRange : public InputActive {
   explicit ActiveMasterRange(InputAction* action, ActiveRange* range,
                             ActiveAxisToRange* axis);
  void Update(float dt);

   ActiveRange* range_;
  ActiveAxisToRange* axis_;
};

}  // namespace input




