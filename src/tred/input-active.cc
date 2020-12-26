// Euphoria - Copyright (c) Gustav

#include "tred/input-active.h"
#include <cassert>

#include "tred/input-action.h"



namespace input {

float KeepWithin(float mi, float v, float ma)
{
    if(v < mi) { return mi;}
    else if(v > ma) { return ma;}
    else return v;
}

InputActive::InputActive(InputAction* action) : action_(action), state_(0.0f) {
  assert(this);
}

float InputActive::state() const {
  assert(this);
  float value = state_;

  switch (action().range()) {
    case Range::INFINITE:
      /* do nothing */
      break;
    case Range::WITHIN_ZERO_ONE:
      value = KeepWithin(0.0f, value, 1.0f);
      break;
    case Range::WITHIN_NEGATIVE_ONE_POSITIVE_ONE:
      value = KeepWithin(-1.0f, value, 1.0f);
      break;
    case Range::INVALID:
    default:
      assert(0 && "Using invalid value");
      break;
  }

  return value;
}

const InputAction& InputActive::action() const {
  assert(this);
  assert(action_);
  return *action_;
}

void InputActive::set_state(float state) {
  assert(this);
  state_ = state;
}

}  // namespace input

