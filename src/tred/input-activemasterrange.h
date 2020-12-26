// Euphoria - Copyright (c) Gustav

/** @file
Classes for input handling.
 */

#ifndef EUPHORIA_INPUT_ACTIVEMASTERRANGE_H_
#define EUPHORIA_INPUT_ACTIVEMASTERRANGE_H_

#include <memory>
#include <cassert>

#include "tred/input-active.h"



namespace input {

struct ActiveRange;
struct ActiveAxisToRange;

struct ActiveMasterRange : public InputActive {
 public:
  explicit ActiveMasterRange(InputAction* action, ActiveRange* range,
                             ActiveAxisToRange* axis);
  void Update(float dt);

 private:
  ActiveRange* range_;
  ActiveAxisToRange* axis_;
};

}  // namespace input



#endif  // EUPHORIA_INPUT_ACTIVEMASTERRANGE_H_
