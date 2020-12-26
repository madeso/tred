// Euphoria - Copyright (c) Gustav

/** @file
Classes for input handling.
 */

#ifndef EUPHORIA_INPUT_ACTIVEMASTERAXIS_H_
#define EUPHORIA_INPUT_ACTIVEMASTERAXIS_H_

#include <memory>
#include <cassert>

#include "tred/input-active.h"



namespace input {

struct ActiveAxis;
struct ActiveRangeToAxis;

struct ActiveMasterAxis : public InputActive {
 public:
  explicit ActiveMasterAxis(InputAction* action, ActiveAxis* axis,
                            ActiveRangeToAxis* range);
  void Update(float dt);

 private:
  ActiveAxis* axis_;
  ActiveRangeToAxis* range_;
};

}  // namespace input



#endif  // EUPHORIA_INPUT_ACTIVEMASTERAXIS_H_
